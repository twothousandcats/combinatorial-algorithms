namespace HopcroftKarp.UI;

public class IOConsole : IConsole
{
    public void Write( string message )
    {
        System.Console.Write( message );
    }

    public void WriteLine( string message )
    {
        System.Console.WriteLine( message );
    }

    public string? ReadLine()
    {
        return System.Console.ReadLine();
    }
}