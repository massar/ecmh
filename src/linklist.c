/* Generic linked list routine.
 * Copyright (C) 1997, 2000 Kunihiro Ishiguro
 */

#include "linklist.h"
#include <string.h>
#include <strings.h>
#include <stdlib.h>

/* Allocate new list. */
struct list *
list_new ()
{
  struct list *new;

  new = malloc (sizeof (struct list));
  memset (new, 0, sizeof (struct list));
  return new;
}

/* Free list. */
void
list_free (struct list *l)
{
  free (l);
}

/* Allocate new listnode.  Internal use only. */
static struct listnode *
listnode_new ()
{
  struct listnode *node;

  node = malloc (sizeof (struct listnode));
  memset (node, 0, sizeof (struct listnode));
  return node;
}

/* Free listnode. */
static void
listnode_free (struct listnode *node)
{
  free (node);
}

/* Add new data to the list. */
void
listnode_add (struct list *list, void *val)
{
  struct listnode *node;

  node = listnode_new ();

  node->prev = list->tail;
  node->data = val;

  if (list->head == NULL)
    list->head = node;
  else
    list->tail->next = node;
  list->tail = node;

  list->count++;
}

/* Add new node with sort function. */
void
listnode_add_sort (struct list *list, void *val)
{
  struct listnode *n;
  struct listnode *new;

  new = listnode_new ();
  new->data = val;

  if (list->cmp)
    {
      for (n = list->head; n; n = n->next)
	{
	  if ((*list->cmp) (val, n->data) < 0)
	    {	    
	      new->next = n;
	      new->prev = n->prev;

	      if (n->prev)
		n->prev->next = new;
	      else
		list->head = new;
	      n->prev = new;
	      list->count++;
	      return;
	    }
	}
    }

  new->prev = list->tail;

  if (list->tail)
    list->tail->next = new;
  else
    list->head = new;

  list->tail = new;
  list->count++;
}

void
listnode_add_after (struct list *list, struct listnode *pp, void *val)
{
  struct listnode *nn;

  nn = listnode_new ();
  nn->data = val;

  if (pp == NULL)
    {
      if (list->head)
	list->head->prev = nn;
      else
	list->tail = nn;

      nn->next = list->head;
      nn->prev = pp;

      list->head = nn;
    }
  else
    {
      if (pp->next)
	pp->next->prev = nn;
      else
	list->tail = nn;

      nn->next = pp->next;
      nn->prev = pp;

      pp->next = nn;
    }
}


/* Delete specific date pointer from the list. */
void
listnode_delete (struct list *list, void *val)
{
  struct listnode *node;

  for (node = list->head; node; node = node->next)
    {
      if (node->data == val)
	{
	  if (node->prev)
	    node->prev->next = node->next;
	  else
	    list->head = node->next;

	  if (node->next)
	    node->next->prev = node->prev;
	  else
	    list->tail = node->prev;

	  list->count--;
	  listnode_free (node);
	  return;
	}
    }
}

/* Delete all listnode from the list. */
void
list_delete_all_node (struct list *list)
{
  struct listnode *node;
  struct listnode *next;

  for (node = list->head; node; node = next)
    {
      next = node->next;
      if (list->del)
	(*list->del) (node->data);
      listnode_free (node);
    }
  list->head = list->tail = NULL;
  list->count = 0;
}

/* Delete all listnode then free list itself. */
void
list_delete (struct list *list)
{
  struct listnode *node;
  struct listnode *next;

  for (node = list->head; node; node = next)
    {
      next = node->next;
      if (list->del)
	(*list->del) (node->data);
      listnode_free (node);
    }
  list_free (list);
}

/* Lookup the node which has given data. */
struct listnode *
listnode_lookup (struct list *list, void *data)
{
  struct listnode *node;

  for (node = list->head; node; nextnode (node))
    if (data == getdata (node))
      return node;
  return NULL;
}

/* Delete the node from list.  For ospfd and ospf6d. */
void
list_delete_node (struct list *list, struct listnode *node)
{
  if (node->prev)
    node->prev->next = node->next;
  else
    list->head = node->next;
  if (node->next)
    node->next->prev = node->prev;
  else
    list->tail = node->prev;
  list->count--;
  listnode_free (node);
}

/* Move the node to the front - for int_find() - jeroen */
void list_movefront_node(struct list *list, struct listnode *node)
{
	// Don't do a thing when it is already there
	if (list->head == node) return;

	// Delete it from the list's current position
	if (node->prev) node->prev->next = node->next;
	else list->head = node->next;
	if (node->next) node->next->prev = node->prev;
	else list->tail = node->prev;

	// Insert it at the front
	if (list->head)
	{
		list->head->prev = node;
		node->prev = list->head->prev;
	}
	node->next = list->head;
	list->head = node;
}

void
list_add_node_prev (struct list *list, struct listnode *current, void *val)
{
  struct listnode *node;

  node = listnode_new ();
  node->next = current;
  node->data = val;

  if (current->prev == NULL)
    list->head = node;
  else
    current->prev->next = node;

  node->prev = current->prev;
  current->prev = node;

  list->count++;
}

void
list_add_node_next (struct list *list, struct listnode *current, void *val)
{
  struct listnode *node;

  node = listnode_new ();
  node->prev = current;
  node->data = val;

  if (current->next == NULL)
    list->tail = node;
  else
    current->next->prev = node;

  node->next = current->next;
  current->next = node;

  list->count++;
}

void
list_add_list (struct list *l, struct list *m)
{
  struct listnode *n;

  for (n = listhead (m); n; nextnode (n))
    listnode_add (l, n->data);
}
