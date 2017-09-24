Network Programming Class in 2017

# TCPEchoClient-Server
> gcc compile
<pre><code>gcc -o Server TCPEchoServer.c HandleTCPClient.c DieWithError.c</code></pre>
<pre><code>gcc -o Client TCPEchoClient.c DieWithError.c</code></pre>
> run echo client-server
<pre><code>./Server 4000 &</code></pre>
<pre><code>./Client 127.0.0.1 "Hello there" 4000</code></pre>

