#include "main.h"

/**
 * delete_node_at_index - deletes a node at given index
 *
 * @head: address of pointer to first node
 * @index: index of node to delete
 *
 * Return: 1 on success, -1 otherwise
 */
int delete_node_at_index(list_t **head, unsigned int index)
{
	list_t *nodeAtIndex, *nodeAtPrevIndex;

	if (head == NULL || *head == NULL)
		return (-1);

	nodeAtIndex = get_node_at_index(*head, index);

	if (nodeAtIndex == NULL)
		return (-1);

	if (index == 0)
	{
		*head = nodeAtIndex->next;
	}
	else 
	{
		nodeAtPrevIndex = get_node_at_index(*head, index - 1);
		nodeAtPrevIndex->next = nodeAtIndex->next;
	}

	nodeAtIndex->next = NULL;
	free(nodeAtIndex);

	return (1);
}

/**
 * delete_node_at_index - deletes a node at given index
 *
 * @head: address of pointer to first node
 * @index: index of node to delete
 *
 * Return: 1 on success, -1 otherwise
 */
int delete_node_at_value(list_t **head, int value)
{
	list_t *nodeAtValue, *nodeAtPrevIndex;
	size_t index;

	if (head == NULL || *head == NULL)
		return (-1);

	nodeAtValue = get_node_at_value(*head, value);

	if (nodeAtValue == NULL)
		return (-1);

	index = get_index_at_value(*head, value);
	if (index == 0)
	{
		*head = nodeAtValue->next;
	}
	else 
	{
		nodeAtPrevIndex = get_node_at_index(*head, index - 1);
		nodeAtPrevIndex->next = nodeAtValue->next;
	}

	nodeAtValue->next = NULL;
	free_node(&nodeAtValue);

	return (1);
}

/**
 * list_length - Calculates the length of a singly linked list
 *
 * @head: pointer to first node
 *
 * Return: list's length
 */
size_t list_length(list_t *head)
{
	size_t i = 0;

	if (head == NULL)
		return (i);

	for (; head != NULL; i++)
		head = head->next;

	return (i);
}

/**
 * get_last_value - Returns the numeric value of the last node.
 *
 * @head: pointer to first node
 *
 * Return: Index of last node
 */
int get_last_value(list_t *head)
{
	if (head == NULL)
		return (0);

	while (head->next != NULL)
		head = head->next;

	return (head->num);
}

/**
 * free_list - frees all nodes of a singly linked list
 *
 * @head: address of pointer to first node
 *
 * Return: nothing
 */
void free_list(list_t **head)
{
	list_t *node, *next_node, *temp;

	if (head == NULL || *head == NULL)
		return;

	temp = *head;
	node = temp;

	while (node)
	{
		next_node = node->next;
		if (node->str)
			free(node->str);
		free(node);
		node = next_node;
	}

	*head = NULL;
}
