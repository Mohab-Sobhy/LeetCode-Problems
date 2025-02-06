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
