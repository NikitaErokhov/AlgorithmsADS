struct Node
{
    Node *left;
    Node *right;
    int value;
    int left_size = 0; // если пустая левая ветвь - 0
};

int GetKOrdStat(Node *head, int K)
{
    if (!head || K<0)
        return -1; // невалидные данные

    Node *curr_node = head;

    while (curr_node)
    {
        if (K < curr_node->left_size)
        {
            curr_node = curr_node->left;
        }
        else if (K == curr_node->left_size)
        {
            return curr_node->value;
        }
        else
        {
            K -= curr_node->left_size + 1;
            curr_node = curr_node->right;
        }
    }

    return -1;  //K>n
}