# fast_calculate_TOM_of_WGCNA
Algorithm optimisation for weighted gene co-expression network analysis(WGCNA, a package of R language): accelerating the calculation of Topology Overlap Matrices(TOM) with C++

<h1>Introduction</h1>

Weighted gene co-expression network analysis(WGCNA) is a R package that is used to analysize the data of gene expression to construct gene regulatory networks. The most time-consuming step of the whole analysis is using TOMsimilarity() function to calculate the Topological Overlap Matrix (TOM) from the Adjacency Matrix in a single thread. TOMsimilarity() function is single-threaded, so I realize multi-threaded calculation in C++. These codes can run on Linux. The following is the usage.

<h1>Usage of codes</h1>

<h2>Step 1: Install SQLite on Linux</h2>
If your operating system is Ubuntu, you can open the terminal and run:   <br/>
  <pre>           sudo apt install sqlite3
  
  </pre>

<h2>Step 2: Compile C++ codes and run</h2>
According to the parameters of TOMsimilarity() function in WGCNA that you want to set up ( usually, TOMType = "unsigned", TOMDenom = "min" ), download the C++ codes named calculate_TOM_unsigned_min.cpp or calculate_TOM_unsigned_mean.cpp and open the terminal to compile them:  <br/>
  &nbsp;&nbsp;&nbsp;&nbsp; g++ -fopenmp calculate_TOM_unsigned_min.cpp -o calculate_TOM_unsigned_min -O3 -lgomp -lpthread -lsqlite3  <br/>
  &nbsp;&nbsp;&nbsp;&nbsp; or g++ -fopenmp calculate_TOM_unsigned_mean.cpp -o calculate_TOM_unsigned_mean -O3 -lgomp -lpthread -lsqlite3  <br/>
Then, run it:   <br/>
  &nbsp;&nbsp;&nbsp;&nbsp; ./calculate_TOM_unsigned_min   <br/>
  &nbsp;&nbsp;&nbsp;&nbsp; or ./calculate_TOM_unsigned_mean   <br/>
C++ will wait for Adjacency Matrix in SQLite database files from R language.   <br/><br/>

<h2>Step 3: run R scripts</h2>
Download the R scripts named calculate_TOM.R and replace the original TOMsimilarity() function in WGCNA.   <br/>
When you figure out the Adjacency Matrix, you can run the R scripts here.    <br/><br/>

<h2>Step 4: wait for the calculation of TOM and release memory</h2>
The R scripts will transfer the Adjacency Matrix to SQLite database files. Then, C++ will read the Adjacency Matrix and calculate the TOM. Next, C++ transfer the TOM into SQLite database files and R read the files. Finally, R combine the results and get TOM.   <br/>
When you get TOM in R, you would better run the following code in terminal to release memory.    <br/>
  &nbsp;&nbsp;&nbsp;&nbsp; rm /dev/shm/calculate_TOM/*   <br/><br/>

<h1>Others</h1>

Details about Weighted gene co-expression network analysis(WGCNA): https://horvath.genetics.ucla.edu/html/CoexpressionNetwork/Rpackages/WGCNA/   <br/>
Details about TOM: https://horvath.genetics.ucla.edu/html/CoexpressionNetwork/Rpackages/WGCNA/TechnicalReports/index.html
