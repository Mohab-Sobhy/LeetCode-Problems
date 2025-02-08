namespace Algorithms;

public class Graph
{
    private List< List< int >> nodes;
    private int numOfNodes;

    public Graph(int numOfNodes)
    {
        this.numOfNodes = numOfNodes;
        nodes = new List< List<int> >(numOfNodes);
        
        for (int i = 0; i < numOfNodes; i++)
        {
            nodes.Add(new List<int>());
        }
    }

    public void addEdge(int node1, int node2)
    {
        nodes[node1].Add(node2);
        nodes[node2].Add(node1);
    }

    public void addNode()
    {
        nodes.Add(new List<int>());
        numOfNodes++;
    }

    public List<int> traverse(int startNode)
    {
        List<int> numbers = new List<int>();
        Queue<int> queue = new Queue<int>();
        Boolean[] visitedArr = new Boolean [numOfNodes];
        queue.Enqueue(startNode);
        visitedArr[startNode] = true;

        while (queue.Count != 0)
        {
            int currentNode = queue.Dequeue();
            numbers.Add(currentNode);
            foreach (int element in nodes[currentNode])
            {
                if (visitedArr[element] == false)
                {
                    queue.Enqueue(element);
                    visitedArr[element] = true;
                }
            }
        }

        return numbers;
    }

    public void print()
    {
        for (int i = 0; i < nodes.Count; i++)
        {
            Console.Write(i + " : ");
            
            for (int j = 0; j < nodes[i].Count; j++)
            {
                Console.Write(nodes[i][j] + " ");
            }

            Console.WriteLine();
        }
    }

}
