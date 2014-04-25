import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintStream;
import java.util.HashMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class FindMethods
{
	private static HashMap<String, String> methodNames = new HashMap<>();
	private static final Pattern readElfLine = Pattern
			.compile("[0-9]*:\\s*0*([1-9a-f][0-9a-f]*)\\s*[0-9]*\\s*FUNC\\s*[^ ]*\\s*[^ ]*\\s*[0-9]*\\s*(.*)$");
	private static final Pattern callPattern = Pattern.compile("tid\\s*([0-9a-f]*)\\s*((entering)|(leaving))\\s*0x([0-9a-f]*)");

	public static void main(String[] args) throws FileNotFoundException,
			IOException
	{

		try (PrintStream out = new PrintStream(new FileOutputStream(
				"/work/cef3/chromium/src/cef/barebones/unobfuscated.txt")))
		{
			try (BufferedReader reader = new BufferedReader(new FileReader(
					"/work/cef3/chromium/src/cef/barebones/methods.txt")))
			{
				String line;
				while ((line = reader.readLine()) != null)
				{
					Matcher matcher = readElfLine.matcher(line);
					if (matcher.find())
					{
						methodNames.put(matcher.group(1), matcher.group(2));
					}
				}
			}

			try (BufferedReader reader = new BufferedReader(new FileReader(
//					"/work/cef3/chromium/src/cef/barebones/log.txt")))
					"/home/thallock/minicom.cap")))
			{
				String line;
				while ((line = reader.readLine()) != null)
				{
					Matcher matcher = callPattern.matcher(line);
					if (!matcher.matches())
					{
						out.println(line);
						continue;
					}
					
					String tid = matcher.group(1);
					String enterExit = matcher.group(2);
					String method = matcher.group(5);

					if (!enterExit.equals("entering") && !enterExit.equals("leaving"))
					{
						throw new IllegalStateException("Please don't print this.");
					}

					String methodName = methodNames.get(method);
					if (methodName == null)
					{
						methodName = "Unable to find " + method;
					}
					out.println(enterExit + " : " + tid + " : " + methodName);
				}
			}
		}
	}
}
