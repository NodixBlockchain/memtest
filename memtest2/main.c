// C program to demonstrate delete operation in binary search tree

#include <base/std_def.h>
#include <base/std_mem.h>
#include <base/mem_base.h>
#include <base/std_str.h>

#include <strs.h>
#include <connect.h>
#include <mem_stream.h>
#include <tpo_mod.h>
#include <fsio.h>

#include <tree.h>



struct node
{
	int key;
	mem_zone_ref left;
	mem_zone_ref right;
	
};

unsigned int aid = 0xFFFFF;


int C_API_FUNC do_mark_node(mem_zone_ref_ptr node, unsigned int scan_id)
{
	struct node *tnode;
	tnode = (struct node *) get_zone_ptr(node, 0);

	if(tnode->left.zone!=PTR_NULL)
		mark_zone(tnode->left.zone , scan_id);

	if (tnode->right.zone != PTR_NULL)
		mark_zone(tnode->right.zone, scan_id);

	return 1;
}

int C_API_FUNC do_free_node(mem_zone_ref_ptr node, unsigned int scan_id)
{
	struct node *tnode;
	
	tnode = (struct node *)get_zone_ptr(node, 0);

	release_zone_ref(&tnode->left);
	release_zone_ref(&tnode->right);

	return 1;
}


// A utility function to create a new BST node
mem_zone_ref newNode(int item)
{
	mem_zone_ref			my_node = { PTR_NULL };
	struct node				tnode;

	tnode.key			= item;
	tnode.left.zone		= PTR_NULL;
	tnode.right.zone	= PTR_NULL;

	allocate_new_zone				(aid, sizeof(struct node), &my_node);
	memcpy_c						(get_zone_ptr(&my_node,0), &tnode, sizeof(struct node));

	if((area_type(aid)&0x10)==0)
		set_zone_free			(&my_node, do_free_node);
	else
		set_zone_free			(&my_node, do_mark_node);

	return my_node;
}


// A utility function to do inorder traversal of BST
void inorder(mem_zone_ref root)
{
	if (root.zone != NULL)
	{
		char buff[16];
		struct node *tnode;
		tnode =(struct node *) get_zone_ptr(&root, 0);
		inorder(tnode->left);
		uitoa_s(tnode->key, buff, 16, 10);
		console_print(buff);
		console_print(" ");
		inorder(tnode->right);
	}
}

/* A utility function to insert a new node with given key in BST */
mem_zone_ref insert(mem_zone_ref_ptr my_node, int key)
{
	struct node *tnode;
	
	/* If the tree is empty, return a new node */
	if (my_node->zone == NULL) return newNode(key);

	tnode = (struct node *)get_zone_ptr(my_node, 0);

	/* Otherwise, recur down the tree */
	if (key < tnode->key)
	{
		mem_zone_ref new_left= insert(&tnode->left, key);
		copy_zone_ref	(&tnode->left, &new_left);
		release_zone_ref(&new_left);
	}
	else
	{
		mem_zone_ref new_right = insert(&tnode->right, key);
		copy_zone_ref(&tnode->right, &new_right);
		release_zone_ref(&new_right);
	}
		
	/* return the (unchanged) node pointer */
	return (*my_node);
}

/* Given a non-empty binary search tree, return the node with minimum
key value found in that tree. Note that the entire tree does not
need to be searched. */
mem_zone_ref minValueNode(mem_zone_ref_ptr my_node)
{
	mem_zone_ref	left = { PTR_NULL };
	struct node *current = (struct node *)get_zone_ptr(my_node, 0);

	/* loop down to find the leftmost leaf */
	while (current->left.zone!=PTR_NULL)
	{
		copy_zone_ref		(my_node ,&current->left);
		release_zone_ref	(&current->left);
		current = (struct node *)get_zone_ptr(my_node, 0);
	}

	return (*my_node);
}

/* Given a binary search tree and a key, this function deletes the key
and returns the new root */
mem_zone_ref deleteNode(mem_zone_ref_ptr root, int key)
{
	// base case
	struct node *tnode;
	if (root->zone == PTR_NULL) {
		return (*root);
	}
	
	/* If the tree is empty, return a new node */
	tnode = (struct node *)get_zone_ptr(root, 0);
	// If the key to be deleted is smaller than the root's key,
	// then it lies in left subtree

	if (key < tnode->key)
	{
		mem_zone_ref new_left = deleteNode(&tnode->left, key);
		if (tnode->left.zone != new_left.zone)
		{
			copy_zone_ref(&tnode->left, &new_left);
			release_zone_ref(&new_left);
		}
	}
	// If the key to be deleted is greater than the root's key,
	// then it lies in right subtree
	else if (key > tnode->key)
	{
		mem_zone_ref new_right = deleteNode(&tnode->right, key);
		if (tnode->right.zone != new_right.zone)
		{
			copy_zone_ref	(&tnode->right, &new_right);
			release_zone_ref(&new_right);
		}
	}

	// if key is same as root's key, then This is the node
	// to be deleted
	else
	{
		mem_zone_ref new_right;
		struct node *tmpnode;
		// node with only one child or no child
		if (tnode->left.zone==PTR_NULL)
		{
			mem_zone_ref tmp = { PTR_NULL };
			copy_zone_ref	(&tmp,&tnode->right);
			release_zone_ref(root);
			return tmp;
		}
		else if (tnode->right.zone == PTR_NULL)
		{
			mem_zone_ref tmp = { PTR_NULL };
			copy_zone_ref	(&tmp, &tnode->left);
			release_zone_ref(root);
			return tmp;
		}

		// node with two children: Get the inorder successor (smallest
		// in the right subtree)

		mem_zone_ref temp = minValueNode(&tnode->right);

		tmpnode = (struct node *)get_zone_ptr(&temp, 0);
		// Copy the inorder successor's content to this node
		tnode->key = tmpnode->key;
		release_zone_ref(&temp);

		//root->key = temp->key;

		new_right	= deleteNode(&tnode->right, tnode->key);
		if (tnode->right.zone != new_right.zone)
		{
			copy_zone_ref	(&tnode->right, &new_right);
			release_zone_ref(&new_right);
		}
	}
	return (*root);
}

mem_zone_ref sortedArrayToBST(int arr[],int start, int end)
{
	mem_zone_ref root = { PTR_NULL };
	struct node *tnode;
	/* If the tree is empty, return a new node */

	/* Base Case */
	if (start > end)
	{
		mem_zone_ref null = { PTR_NULL };
		return null;
	}
		

	/* Get the middle element and make it root */
	int mid = (start + end) / 2;
		
	root = newNode(arr[mid]);
	
	tnode = (struct node *)get_zone_ptr(&root, 0);

	/* Recursively construct the left subtree and make it
	left child of root */
	tnode->left = sortedArrayToBST	(arr, start, mid - 1);
	

	/* Recursively construct the right subtree and make it
	right child of root */
	tnode->right = sortedArrayToBST	(arr, mid + 1, end);

	return root;
}


void generate_tree(mem_zone_ref_ptr rootNode,size_t num)
{
	int *vals;
	int n;

	vals = malloc_c(num);
	for (n = 0; n<num; n++)
	{
		vals[n] = n;
	}
	(*rootNode) = sortedArrayToBST(vals, 0, num - 1);
	free_c(vals);
}

OS_API_C_FUNC(int) app_init(mem_zone_ref_ptr params)
{

	tree_manager_init(32 * 1024 * 1024, MEM_TYPE_TREE|0x10);
	return 1;
}	


OS_API_C_FUNC(int) app_start(mem_zone_ref_ptr params)
{
	return 1;
}

OS_API_C_FUNC(int) app_loop(mem_zone_ref_ptr params)
{
	mem_zone_ref	rootNode = { PTR_NULL };
	mem_zone_ref	log = { PTR_NULL };
	int				cnt,icnt;
	int				incr;
	size_t			nz,nnz;
	ctime_t			start,tree_end,end,elapsed;
	

	aid		= get_tree_mem_area_id();
	cnt		= 32*1024;
	incr	= 128;

	get_system_time_c	(&start);
	generate_tree		(&rootNode,cnt);
	get_system_time_c	(&tree_end);

	do_mark_sweep		(aid, 0);
	//inorder			(rootNode);
	//console_print		("\n");
	
	nz = find_zones_used			(aid);

	tree_manager_create_node		("log", NODE_LOG_PARAMS, &log);
	tree_manager_set_child_value_i32(&log, "nz", nz);
	tree_manager_set_child_value_i32(&log, "et", tree_end-start);
	log_message						("generated tree %nz% zones in %et% ms ", &log);
	release_zone_ref				(&log);

	log.zone = PTR_NULL;

	/*
	nz = find_zones_used(aid);
	tree_manager_dump_node_rec(&root, 0, 16);
	deleteNode(&root, 5);
	tree_manager_dump_node_rec(&root, 0, 16);
	nz = find_zones_used(aid);
	*/

	cnt--;

	while (cnt >= 0)
	{
		for (icnt=0;icnt<incr;icnt++)
		{
			mem_zone_ref new_root = { PTR_NULL };

			new_root = deleteNode(&rootNode, cnt-icnt);

			if (new_root.zone != PTR_NULL)
			{
				if (rootNode.zone != new_root.zone)
				{
					copy_zone_ref(&rootNode, &new_root);
					release_zone_ref(&new_root);
				}
			}
			else
			{
				release_zone_ref(&rootNode);
				rootNode.zone = PTR_NULL;
			}
			new_root.zone = PTR_NULL;

		}
		
		cnt -= incr;
		do_mark_sweep(aid,0);

		/*
		nnz = find_zones_used(aid);
		get_system_time_c(&end);
		
		tree_manager_create_node("log", NODE_LOG_PARAMS, &log);
		tree_manager_set_child_value_i32(&log, "nz", nz-nnz);
		tree_manager_set_child_value_i32(&log, "et", end - tree_end);
		log_message("deleted %nz% nodes in %et% ms ", &log);
		release_zone_ref(&log);
		log.zone = PTR_NULL;
		nz = nnz;
		*/

		/*
		inorder(rootNode);
		console_print("\n");
		*/
	}
	
	

	nz = find_zones_used			(aid);
	get_system_time_c				(&end);
	tree_manager_create_node		("log", NODE_LOG_PARAMS, &log);
	tree_manager_set_child_value_i32(&log, "nz", nz);
	tree_manager_set_child_value_i32(&log, "et", end - tree_end);
	log_message						("deleted tree %nz% zones left in %et% ms ", &log);
	release_zone_ref				(&log);


	return 1;
}


OS_API_C_FUNC(int) app_stop(mem_zone_ref_ptr params)
{
	return 1;
}

C_EXPORT mod_name_decoration_t	 mod_name_deco_type = MOD_NAME_DECO;


#ifdef _WIN32
unsigned int C_API_FUNC _DllMainCRTStartup(unsigned int *prev, unsigned int *cur, unsigned int *xx) { return 1; }
#endif
