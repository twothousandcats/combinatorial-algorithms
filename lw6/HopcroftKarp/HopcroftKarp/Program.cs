using HopcroftKarp.UI;

namespace HopcroftKarp;

public class Program
{
    private const string InputFileName = "input.txt";

    public static int Main()
    {
        IConsole console = new IOConsole();
        try
        {
            BipartiteGraph bipartiteGraph = ReadGraph();
            HopcroftKarpMatcher matcher = new HopcroftKarpMatcher( bipartiteGraph );
            Matching result = matcher.FindMaxChain();
            PrintResult( result, console );

            return 0;
        }
        catch ( FormatException ex )
        {
            console.WriteLine( $"invalid format: {ex.Message}" );
            return 2;
        }
        catch ( IOException ex )
        {
            console.WriteLine( $"IO error: {ex.Message}" );
            return 3;
        }
    }

    private static BipartiteGraph ReadGraph()
    {
        using StreamReader streamReader = new StreamReader( InputFileName );
        IEnumerable<string> lines = ReadValues( streamReader );
        using IEnumerator<string> iterator = lines.GetEnumerator();
        ( int n, int expectedEdges ) = ReadHeader( iterator );
        BipartiteGraph bipartiteGraph = new BipartiteGraph( n, n );

        int actualEdges = 0;
        while ( iterator.MoveNext() )
        {
            ( int leftVertex, int rightVertex ) = ParseEdge( iterator.Current );
            bipartiteGraph.AddEdge( leftVertex, rightVertex );
            actualEdges++;
        }

        if ( actualEdges != expectedEdges )
        {
            throw new FormatException( $"Edge count mismatch: header={expectedEdges}, actual={actualEdges}" );
        }

        return bipartiteGraph;
    }

    private static IEnumerable<string> ReadValues( StreamReader reader )
    {
        string? line;
        while ( ( line = reader.ReadLine() ) != null )
        {
            line = line.Trim();
            if ( line.Length == 0 )
            {
                continue;
            }

            yield return line;
        }
    }

    private static (int n, int edgeCount) ReadHeader( IEnumerator<string> iterator )
    {
        if ( !iterator.MoveNext() )
        {
            throw new FormatException( "Header line missing!" );
        }

        string[] parts = SplitTokens( iterator.Current );
        if ( parts.Length != 2 )
        {
            throw new FormatException( $"Header must contain 2 numbers: {iterator.Current}" );
        }

        int count = ParseNonNegative( parts[ 0 ] );
        int edgeCount = ParseNonNegative( parts[ 1 ] );
        if ( count == 0 )
        {
            throw new FormatException( "Vertex count have to be greater than zero!" );
        }

        return (
            count,
            edgeCount
        );
    }

    private static (int u, int v) ParseEdge( string line )
    {
        string[] parts = SplitTokens( line );
        if ( parts.Length != 2 )
        {
            throw new FormatException( $"Edge must contain 2 numbers: {line}" );
        }

        return (
            ParseNonNegative( parts[ 0 ] ),
            ParseNonNegative( parts[ 1 ] )
        );
    }

    private static string[] SplitTokens( string line )
    {
        return line.Split(
            new[] { ' ', '\t' },
            StringSplitOptions.RemoveEmptyEntries
        );
    }

    private static int ParseNonNegative( string token )
    {
        if ( !int.TryParse( token, out int value ) || value < 0 )
        {
            throw new FormatException( $"Value '{token}' is not a valid integer" );
        }

        return value;
    }

    private static void PrintResult( Matching result, IConsole console )
    {
        console.WriteLine( $"Pairs count: {result.Pairs.Count}" );
        console.WriteLine( $"is complete: {result.IsPerfect}" );
        List<string> chain = new List<string>( result.Pairs.Count * 2 );
        foreach ( KeyValuePair<int, int> pair in result.Pairs )
        {
            console.WriteLine( $"chain: X{pair.Key} -> Y{pair.Value}" );
            // chain.Add( $"X{pair.Key}" );
            // chain.Add( $"Y{pair.Value}" );
        }

        console.WriteLine( "" );
        console.WriteLine( string.Join( " -> ", chain ) );
    }
}