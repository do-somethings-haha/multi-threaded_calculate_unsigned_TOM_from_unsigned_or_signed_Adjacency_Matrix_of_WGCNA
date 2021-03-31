### Before executing the following code, 
### you must have got the unsigned or signed Adjacency Matrix named adjacency_matrix. "unsigned" is the default value of the parameter named type of the "adjacency" function in the WGCNA.

### install and import the Rcpp amd RcppArmadillo packages
# install.packages( c("Rcpp" , "RcppArmadillo") )
library(Rcpp)
library(RcppArmadillo)
RcppArmadillo.package.skeleton("FastCalculateTOM") # If you have run this line of code, there is already a folder named FastCalculateTOM in the working directory of R, then you don’t need to run this line of code. 

### Put the C++ source files in the current working directory of R, 
### then compile and import the C++ function
# getwd()  # Look at the current working directory 
Rcpp::sourceCpp( "FastCalculateTOM.cpp" )

### run the C++ function, and get the TOM
TOM = calculate_unsigned_TOM_from_unsigned_or_signed_Adjacency_Matrix( adjacency_matrix )
