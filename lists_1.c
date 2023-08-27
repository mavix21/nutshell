#include "main.h"

/**
 * set_node - initializes a new node of type list_t
 *
 * @str: string value of the node
 * @index: node index
 *
 * Return: pointer to the initialized new node
 */
list_t *set_node(const char *str, int index)
{
	list_t *new_node = malloc(sizeof(list_t));
	if (new_node == NULL)
		return (NULL);

	memset((void *)new_node, 0, sizeof(list_t));
	new_node->num = index;

	if (str == NULL)
	{
		new_node->str = NULL;
		return (new_node);
	}

	new_node->str = strdup(str);
	if (new_node->str == NULL)
	{
		free(new_node);
		return (NULL);
	}

	return (new_node);
}

/**
 * add_node - adds a new node to the start of a singly linked list
 *
 * @head: address of the pointer to the head node
 * @str: string value of the node
 * @index: node index
 *
 * Return: pointer to the added new node
 */
list_t *add_node(list_t **head, const char *str, int index)
{
	list_t *new_node = NULL;

	if (head == NULL)
		return (NULL);

	new_node = set_node(str, index);
	if (new_node == NULL)
		return (NULL);

	new_node->next = *head;
	*head = new_node;

	return (new_node);
}

/**
 * add_node_end - adds a new node to the end of a singly linked list
 *
 * @head: address of the pointer to the head node
 * @str: string value of the node
 * @index: node index
 *
 * Return: pointer to the added new node
 */
list_t *add_node_end(list_t **head, const char *str, int index)
{
	list_t *new_node, *temp;

	if (head == NULL)
		return (NULL);

	new_node = set_node(str, index);
	if (new_node == NULL)
		return (NULL);

	if (*head == NULL)
	{
		*head = new_node;
		return (new_node);
	}

	temp = *head;
	while (temp->next != NULL)
		temp = temp->next;

	temp->next = new_node;

	return (new_node);
}

/**
 * print_list_str - prints the string value of each linked list node
 *
 * @node: pointer to first node of the singly linked list
 *
 * Return: size of the list
 */
size_t print_list_str(const list_t *node)
{
	size_t i = 0;

	while (node != NULL)
	{
		printf("%s\n", node->str ? node->str : "(nil)");
		node = node->next;
		i++;
	}

	return (i);
}

/**
 * get_node_at_index - returns the nth node of a list_t linked list
 *
 * @head: head pointer
 * @index: node index to return
 *
 * Return: pointer to the nth node of the list
 */
list_t *get_node_at_index(list_t *head, unsigned int index)
{
	unsigned int i;

	for (i = 0; i < index && head != NULL; i++)
		head = head->next;

	return (head);
}

/**
 * get_node_at_value - returns the node whose index is equal to value
 *
 * @head: head pointer
 * @value: value that has to be contained in the searched node
 *
 * Return: pointer to node with index equal to value
 */
list_t *get_node_at_value(list_t *head, int value)
{
	while (head != NULL)
	{
		if (head->num == value)
			return (head);

		head = head->next;
	}

	return (NULL);
}

/**
 * get_index_at_value - returns the node index whose value is equal to value
 *
 * @head: head pointer
 * @value: value that has to be contained in the searched node
 *
 * Return: index of node with value equal to value, -1 if it is not present
 */
size_t get_index_at_value(list_t *head, int value)
{
	size_t i;

	for (i = 0; head != NULL; i++)
	{
		if (head->num == value)
			return (i);

		head = head->next;
	}

	return (-1);
}
