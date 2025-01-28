namespace Algorithms;

public class BinarySearch
{
    //Recursive Method
    public static int find<T>(T[] arr , T value , int left , int right) where T : IComparable<T>
    {
        if (left <= right)
        {
            int mid = ((right - left) / 2) + left;
    
            if (arr[mid].CompareTo(value) == 0)
            {
                return mid;
            }
            
            if (arr[mid].CompareTo(value) > 0)
            {
                return find<T>(arr, value, left, mid - 1);
            }
            
            return find<T>(arr, value, mid+1 , right);
        }
    
        return -1;
    }
    
    //Recursive Method
    public static int findIterative<T>(T[] arr, T value, int start, int end) where T : IComparable<T>
    {
        while (start <= end)
        {
            int mid = ((end - start) / 2) + start;
            if (arr[mid].CompareTo(value) == 0)
            {
                return mid;
            }
            else if (arr[mid].CompareTo(value) > 0)
            {
                end = mid - 1;
            }
            else
            {
                start = mid + 1;
            }
        }
        
        return -1;
    }
}
