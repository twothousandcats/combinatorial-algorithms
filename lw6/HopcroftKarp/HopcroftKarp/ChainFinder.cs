namespace HopcroftKarp;

public sealed class ChainFinder
{
    private readonly BipartiteGraph _graph;

    private readonly int[] _pairLeft;

    private readonly int[] _pairRight;

    public ChainFinder( BipartiteGraph graph, Matching matching )
    {
        ArgumentNullException.ThrowIfNull( graph );
        ArgumentNullException.ThrowIfNull( matching );

        _graph = graph;
        _pairLeft = new int[ graph.LeftSize ];
        _pairRight = new int[ graph.RightSize ];
        Array.Fill( _pairLeft, -1 );
        Array.Fill( _pairRight, -1 );
        foreach ( KeyValuePair<int, int> pair in matching.Pairs )
        {
            _pairLeft[ pair.Key ] = pair.Value;
            _pairRight[ pair.Value ] = pair.Key;
        }
    }

    // List of vertexes
    // returns longest chain
    public IReadOnlyList<(bool isLeft, int index)> FindLongest()
    {
        List<(bool, int)> best = new List<(bool, int)>();
        List<(bool, int)> current = new List<(bool, int)>();
        bool[] visitedLeft = new bool[ _graph.LeftSize ];
        bool[] visitedRight = new bool[ _graph.RightSize ];

        for ( int u = 0; u < _graph.LeftSize; u++ )
        {
            if ( _pairLeft[ u ] != -1 )
            {
                continue;
            }

            visitedLeft[ u ] = true;
            current.Add( ( true, u ) );
            Dfs( u, visitedLeft, visitedRight, current, best );
            current.RemoveAt( current.Count - 1 );
            visitedLeft[ u ] = false;
        }

        return best;
    }

    private void Dfs(
        int u,
        bool[] visitedLeft,
        bool[] visitedRight,
        List<(bool, int)> current,
        List<(bool, int)> best )
    {
        bool extended = false;
        foreach ( int v in _graph.GetNeighbors( u ) )
        {
            if ( visitedRight[ v ] )
            {
                continue;
            }

            int next = _pairRight[ v ];
            if ( next == u || visitedLeft[ Math.Max( next, 0 ) ] && next != -1 )
            {
                continue;
            }

            extended = true;
            visitedRight[ v ] = true;
            current.Add( ( false, v ) );

            if ( next == -1 )
            {
                if ( current.Count > best.Count )
                {
                    best.Clear();
                    best.AddRange( current );
                }
            }
            else
            {
                visitedLeft[ next ] = true;
                current.Add( ( true, next ) );
                Dfs( next, visitedLeft, visitedRight, current, best );
                current.RemoveAt( current.Count - 1 );
                visitedLeft[ next ] = false;
            }

            current.RemoveAt( current.Count - 1 );
            visitedRight[ v ] = false;
        }

        if ( !extended && current.Count > best.Count )
        {
            best.Clear();
            best.AddRange( current );
        }
    }
}