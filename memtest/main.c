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
// C program to demonstrate delete operation in binary search tree
#include<stdio.h>
#include<stdlib.h>

/*
struct node
{
	int key;
	struct node *left, *right;
};
*/


// A utility function to create a new BST node
mem_zone_ref newNode(int item)
{
	mem_zone_ref node = { PTR_NULL };

	tree_manager_create_node		("node", NODE_GFX_SIGNED_INT, &node);
	tree_manager_write_node_dword	(&node, 0, item);

	/*
	struct node *temp = (struct node *)malloc(sizeof(struct node));
	temp->key = item;
	temp->left = temp->right = NULL;
	*/
	return node;
}

/*
// A utility function to do inorder traversal of BST
void inorder(struct node *root)
{
	if (root != NULL)
	{
		inorder(root->left);
		printf("%d ", root->key);
		inorder(root->right);
	}
}
*/
/* A utility function to insert a new node with given key in BST */
void insert(mem_zone_ref_ptr node, int key)
{
	int node_key;
	/* If the tree is empty, return a new node */
	//if (node->zone == NULL) return newNode(key);

	tree_mamanger_get_node_dword(node, 0, &node_key);

	/* Otherwise, recur down the tree */
	if (key < node_key)
	{
		mem_zone_ref left = { PTR_NULL };

		if (!tree_manager_find_child_node(node, NODE_HASH("left"), NODE_GFX_SIGNED_INT, &left))
		{
			tree_manager_add_child_node		(node, "left", NODE_GFX_SIGNED_INT, &left);
			tree_manager_write_node_dword	(&left, 0, key);
		}
		else
		{
			insert(&left, key);
		}
		release_zone_ref(&left);
	}
	else
	{
		mem_zone_ref right = { PTR_NULL };

		if (!tree_manager_find_child_node(node, NODE_HASH("right"), NODE_GFX_SIGNED_INT, &right))
		{
			tree_manager_add_child_node		(node, "right", NODE_GFX_SIGNED_INT, &right);
			tree_manager_write_node_dword	(&right, 0, key);
		}
		else
		{
			insert(&right, key);
		}
		release_zone_ref(&right);
	}
		

	/* return the (unchanged) node pointer */
	return;
}

/* Given a non-empty binary search tree, return the node with minimum
key value found in that tree. Note that the entire tree does not
need to be searched. */
mem_zone_ref minValueNode(mem_zone_ref_ptr node)
{
	mem_zone_ref	left = { PTR_NULL };
	mem_zone_ref_ptr current = node;

	/* loop down to find the leftmost leaf */
	while (tree_manager_find_child_node(current, NODE_HASH("left"), NODE_GFX_SIGNED_INT, &left))
	{
		current = &left;
		release_zone_ref(&left);
	}

	return (*current);
}

/* Given a binary search tree and a key, this function deletes the key
and returns the new root */
mem_zone_ref deleteNode(mem_zone_ref_ptr root, int key)
{
	// base case
	int node_key;
	if (root->zone == NULL) {
		mem_zone_ref null = { PTR_NULL };
		return null;
	}

	// If the key to be deleted is smaller than the root's key,
	// then it lies in left subtree

	tree_mamanger_get_node_dword(root, 0, &node_key);

	if (key < node_key)
	{
		mem_zone_ref new_left = { PTR_NULL },left = { PTR_NULL };

		tree_manager_find_child_node(root, NODE_HASH("left"), NODE_GFX_SIGNED_INT, &left);
		new_left = deleteNode		(&left, key);
		
		if (new_left.zone != left.zone)
		{
			tree_manager_set_node_name	(&new_left, "left");
			tree_manager_swap_child_ref	(root, NODE_HASH("left"), NODE_GFX_SIGNED_INT, &new_left);
			release_zone_ref			(&new_left);
		}

		release_zone_ref			(&left);
		

		//root->left = deleteNode(root->left, key);
	}

	// If the key to be deleted is greater than the root's key,
	// then it lies in right subtree
	else if (key > node_key)
	{
		mem_zone_ref new_right = { PTR_NULL }, right = { PTR_NULL };
		tree_manager_find_child_node(root, NODE_HASH("right"), NODE_GFX_SIGNED_INT, &right);

		new_right = deleteNode		(&right, key);
		if (new_right.zone != right.zone)
		{
			tree_manager_set_node_name (&new_right, "right");
			tree_manager_swap_child_ref(root, NODE_HASH("right"), NODE_GFX_SIGNED_INT, &new_right);
			release_zone_ref(&new_right);
		}
		release_zone_ref	(&right);
	}

	// if key is same as root's key, then This is the node
	// to be deleted
	else
	{
		mem_zone_ref new_right = { PTR_NULL }, right = { PTR_NULL };
		// node with only one child or no child
		if (!tree_manager_find_child_node(root, NODE_HASH("left"), NODE_GFX_SIGNED_INT, PTR_NULL))
		{
			mem_zone_ref temp = { PTR_NULL };
			tree_manager_find_child_node	(root, NODE_HASH("right"), NODE_GFX_SIGNED_INT, &temp);
			return temp;
		}
		else if (!tree_manager_find_child_node(root, NODE_HASH("right"), NODE_GFX_SIGNED_INT, PTR_NULL))
		{
			mem_zone_ref temp = { PTR_NULL };
			tree_manager_find_child_node	(root, NODE_HASH("left"), NODE_GFX_SIGNED_INT, &temp);
			return temp;
		}

		// node with two children: Get the inorder successor (smallest
		// in the right subtree)

		tree_manager_find_child_node(root, NODE_HASH("right"), NODE_GFX_SIGNED_INT, &right);

		mem_zone_ref temp = minValueNode(&right);

		// Copy the inorder successor's content to this node

		tree_mamanger_get_node_dword	(&temp, 0, &node_key);
		tree_manager_write_node_dword	(root, 0, node_key);
		release_zone_ref				(&temp);

		//root->key = temp->key;
		
		tree_manager_find_child_node	(root, NODE_HASH("right"), NODE_GFX_SIGNED_INT, &right);
		new_right =deleteNode			(&right, node_key);
		
		if (new_right.zone != right.zone)
		{
			tree_manager_set_node_name	(&new_right, "right");
			tree_manager_swap_child_ref	(root, NODE_HASH("right"), NODE_GFX_SIGNED_INT, &new_right);
			release_zone_ref			(&new_right);
		}
		release_zone_ref				(&right);
		

		// Delete the inorder successor
		//root->right = deleteNode(root->right, temp->key);
	}
	return (*root);
}

mem_zone_ref sortedArrayToBST(int arr[], const char *name,int start, int end)
{
	mem_zone_ref left = { PTR_NULL }, right = { PTR_NULL }, root = { PTR_NULL };
	/* Base Case */
	if (start > end)
	{
		mem_zone_ref null = { PTR_NULL };
		return null;
	}
		

	/* Get the middle element and make it root */
	int mid = (start + end) / 2;
	
	if(name==PTR_NULL)
		root = newNode(arr[mid]);
	else
	{
		tree_manager_create_node		(name, NODE_GFX_SIGNED_INT, &root);
		tree_manager_write_node_dword	(&root, 0, arr[mid]);
	}

	/* Recursively construct the left subtree and make it
	left child of root */
	left = sortedArrayToBST			(arr,"left", start, mid - 1);
	tree_manager_node_add_child		(&root, &left);
	release_zone_ref				(&left);
	

	/* Recursively construct the right subtree and make it
	right child of root */
	right = sortedArrayToBST		(arr, "right", mid + 1, end);
	tree_manager_node_add_child		(&root, &right);
	release_zone_ref				(&right);

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
	(*rootNode) = sortedArrayToBST(vals, PTR_NULL, 0, num - 1);
	free_c(vals);

	//tree_manager_dump_node_rec(&root, 0, 16);

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
	unsigned int	aid;
	size_t			nz,nnz;
	ctime_t			start,tree_end,end,elapsed;


	

	aid		= get_tree_mem_area_id();
	cnt		= 32*1024;
	incr	= 128;

	get_system_time_c	(&start);
	generate_tree		(&rootNode,cnt);
	get_system_time_c	(&tree_end);

	do_mark_sweep		(aid, 0);

	

	//tree_manager_dump_node_rec		(&root, 0, 16);
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
				rootNode.zone = PTR_NULL;

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
		nz = nnz;
		*/
	}
	
	

	nz = find_zones_used			(aid);
	get_system_time_c				(&end);
	tree_manager_create_node		("log", NODE_LOG_PARAMS, &log);
	tree_manager_set_child_value_i32(&log, "nz", nz);
	tree_manager_set_child_value_i32(&log, "et", end - tree_end);
	log_message						("deleted tree %nz% zones left in %et% ms ", &log);
	release_zone_ref				(&log);

	//tree_manager_dump_node_rec(&root, 0, 16);


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
