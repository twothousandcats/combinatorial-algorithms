namespace HopcroftKarp;

public sealed class BipartiteGraph
{
    private readonly List<int>[] _adjacencyList;

    public BipartiteGraph( int leftSize, int rightSize )
    {
        if ( leftSize <= 0 || rightSize <= 0 )
        {
            throw new ArgumentOutOfRangeException(
                $"{nameof( leftSize )} and {nameof( rightSize )}  must be greater than 0"
            );
        }

        LeftSize = leftSize;
        RightSize = rightSize;
        _adjacencyList = new List<int>[ leftSize ];
        for ( int i = 0; i < leftSize; i++ )
        {
            _adjacencyList[ i ] = new List<int>();
        }
    }

    public int LeftSize { get; }

    public int RightSize { get; }

    public void AddEdge( int left, int right )
    {
        if ( left < 0 || left >= LeftSize )
        {
            throw new ArgumentOutOfRangeException(
                $"{left} must be greater than 0 and equal to {LeftSize}"
            );
        }

        if ( right < 0 || right >= RightSize )
        {
            throw new ArgumentOutOfRangeException(
                $"{right} must be greater than 0 and equal to {RightSize}"
            );
        }

        _adjacencyList[ left ].Add( right );
    }

    public IReadOnlyList<int> GetNeighbors( int left )
    {
        return _adjacencyList[ left ];
    }
}