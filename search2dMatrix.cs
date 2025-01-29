namespace Algorithms;

public static class Solution {
    public static bool SearchMatrix(int[][] matrix, int target)
    {
        int start = 0;
        int end = matrix.GetLength(0) - 1;
        int rowInd = matrix.GetLength(0);

        while (start <= end)
        {
            int mid = ((end - start) / 2) + start;

            if (matrix[mid][0] > target)
            {
                //target less than or equal mid
                rowInd = mid;
                end = mid - 1;
            }
            else
            {
                start = mid + 1;
            }
        }

        if (rowInd == 0 || Array.BinarySearch(matrix[rowInd - 1], target) < 0)
        {
            return false;
        }
        else
        {
            return true;
        }
        

    }
    
}
