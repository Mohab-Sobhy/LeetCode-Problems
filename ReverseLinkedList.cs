static void reverseLinkedList(LinkedList<int> linkedList)
    {
        LinkedListNode<int>? current = linkedList.First;
        LinkedListNode<int>? next = null;

        while (current != null)
        {
            next = current.Next;
            linkedList.Remove(current);
            linkedList.AddFirst(current);
            current = next;
        }
        
    }
