# fast_calculate_TOM_of_WGCNA
Algorithm optimisation for weighted gene co-expression network analysis(WGCNA, a package of R language): accelerating the calculation of Topology Overlap Matrices(TOM) with C++

<h1>Introduction</h1>

Weighted gene co-expression network analysis(WGCNA) is a R package that is used to analysize the data of gene expression to construct gene regulatory networks. The most time-consuming step of the whole analysis is using TOMsimilarity() function to calculate the Topological Overlap Matrix (TOM) from the Adjacency Matrix in a single thread. TOMsimilarity() function is single-threaded, so I realize multi-threaded calculation in C++. These codes can run on Linux. The following is the usage.

<h1>Usage of codes</h1>

<h2>Step 1: Install SQLite on Linux</h2>
If your operating system is Ubuntu, you can open the terminal and run:   <br/>
  <pre>sudo apt-get install sqlite3
sudo apt-get install libsqlite3-dev</pre>

<h2>Step 2: Compile C++ codes and run</h2>
According to the parameters of TOMsimilarity() function in WGCNA that you want to set up ( usually, TOMType = "unsigned", TOMDenom = "min" ), download the C++ codes named calculate_TOM_unsigned_min.cpp or calculate_TOM_unsigned_mean.cpp and open the terminal to compile them:  <br/>
  <pre>g++ -fopenmp calculate_TOM_unsigned_min.cpp -o calculate_TOM_unsigned_min -O3 -lgomp -lpthread -lsqlite3
or   g++ -fopenmp calculate_TOM_unsigned_mean.cpp -o calculate_TOM_unsigned_mean -O3 -lgomp -lpthread -lsqlite3</pre>
Then, run it:   <br/>
  <pre>./calculate_TOM_unsigned_min
or   ./calculate_TOM_unsigned_mean</pre>
C++ will wait for Adjacency Matrix in SQLite database files from R language.   <br/><br/>

<h2>Step 3: run R scripts</h2>
Download the R scripts named calculate_TOM.R and replace the original TOMsimilarity() function in WGCNA.    <br/>
Then, you figure out the Adjacency Matrix by adjacency function in WGCNA, and get a matrix named adjacency_matrix.    <br/>
Next, you can run the R scripts here instead of TOMsimilarity() function.     <br/><br/>

<h2>Step 4: wait for the calculation of TOM and release memory</h2>
The R scripts will transfer the Adjacency Matrix to SQLite database files. Then, C++ will read the Adjacency Matrix and calculate the TOM. Next, C++ transfer the TOM into SQLite database files and R read the files. Finally, R combine the results and get TOM.   <br/>
When you get TOM in R, you can run the following code in terminal to release memory if there are not other programs creating files in /dev/shm/.    <br/>
  <pre>rm /dev/shm/*</pre>

<h1>Others</h1>

Details about Weighted gene co-expression network analysis(WGCNA):  <br/>
  &nbsp;&nbsp;&nbsp;&nbsp;  https://horvath.genetics.ucla.edu/html/CoexpressionNetwork/Rpackages/WGCNA/   <br/>
Details about the usage of original TOMsimilarity() function   <br/>
  &nbsp;&nbsp;&nbsp;&nbsp;  https://horvath.genetics.ucla.edu/html/CoexpressionNetwork/Rpackages/WGCNA/Tutorials/FemaleLiver-02-networkConstr-man.pdf  <br/>
Details about mathematical formulas of the Topological Overlap Matrix (TOM): <br/>
  &nbsp;&nbsp;&nbsp;&nbsp;  https://horvath.genetics.ucla.edu/html/CoexpressionNetwork/Rpackages/WGCNA/TechnicalReports/index.html

