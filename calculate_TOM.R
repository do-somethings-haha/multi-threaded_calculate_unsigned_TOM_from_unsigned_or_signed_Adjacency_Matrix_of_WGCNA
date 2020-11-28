# These codes are followed by the calculation of the softPower. 
#   Details about softPower are in 2.b.1 of https://horvath.genetics.ucla.edu/html/CoexpressionNetwork/Rpackages/WGCNA/Tutorials/FemaleLiver-02-networkConstr-man.pdf

# Before the following calculations, the soft threshold named softPower must be determined,
# and we must have a expression matrix without abnormal samples named datExpr whose row names are sample names and column names are gene names )
adjacency_matrix = adjacency( datExpr , power = softPower )
gc()
        
 
# The following codes are will calculate the Topological Overlap Matrix (TOM).
#   The following codes will replace TOMsimilarity() function in WGCNA packages and also figure out a matrix named TOM with colnames "V1,V2,V3...".
#     Two kinds of TOMs differ only in the last significant figures due to the truncation error.
#   You can fold the codes at the braces.
{
  start_time_of_whole_TOM_calculation = Sys.time( )
  
  # write the Adjacency Matrix into SQLite database files
  {
    # install.packages( c("DBI", "RSQLite","foreach","doParallel") )
    library(DBI)
    library(RSQLite)
    library(foreach)
    library(doParallel)
    total_number_of_cores  =  detectCores( ) 
    connecting_to_sqlite_database  =  dbConnect(  RSQLite::SQLite( ) , 
                                                  dbname = "/dev/shm/sqlite_database_file_in_memory" )
    dbSendQuery(   connecting_to_sqlite_database   ,   "PRAGMA  synchronous = OFF ;"  )
    
    # clear the database file
    {
      table_names_vector = dbListTables(  connecting_to_sqlite_database  ) 
      for( current_table_name  in  table_names_vector ){
        sql_string_delete_table = paste( "DROP TABLE " ,
                                         current_table_name ,
                                         sep="")
        dbSendQuery( connecting_to_sqlite_database , sql_string_delete_table)
      }
    }
    
    # write total_gene_number into the database file
    {
      total_gene_number = data.frame( total_gene_number = c( ncol(adjacency_matrix) ))
      dbWriteTable(  connecting_to_sqlite_database  , 
                     "total_gene_number"  ,  
                     total_gene_number  )
    }
    
    
    # clear many database files that will be used
    {
      for( current_database_number_saving_adjacency in 1:( ceiling(total_gene_number[[1,1]]/2000) ) )
      {
        current_table_name_also_the_database_name_in_sqlite = paste( "the_" ,
                                                                     current_database_number_saving_adjacency ,
                                                                     "_part_of_adjaceny_table_in_sqlite_database" ,
                                                                     sep="")
        connecting_to_sqlite_database_of_current_process  =  dbConnect(  RSQLite::SQLite( ) ,
                                                                         dbname = paste("/dev/shm/" ,
                                                                                        current_table_name_also_the_database_name_in_sqlite ,
                                                                                        sep="") )
        table_names_vector_of_current_database = dbListTables(  connecting_to_sqlite_database_of_current_process  ) 

        for( current_table_name_of_current_database  in  table_names_vector_of_current_database ){
          sql_string_delete_table = paste( "DROP TABLE " ,
                                           current_table_name_of_current_database ,
                                           sep="")
          dbSendQuery( connecting_to_sqlite_database_of_current_process , sql_string_delete_table)
        }
        dbDisconnect(  connecting_to_sqlite_database_of_current_process  )
      }
      
    }
    
    #  split the Adjacency Matrix into many data frames with a maximum column number of 2000, and write them into SQLite database files 
    start_time_write_adjacency_to_sqlite  =  Sys.time( )
    {
      # split the Adjacency Matrix into many data frames with a maximum column number of 2000
      {
        adjacency_matrix = as.data.frame( adjacency_matrix ) 
        gc()
        total_column_number_of_adjaceny = ncol( adjacency_matrix )
        total_table_number_in_sqlite_database = ceiling( total_column_number_of_adjaceny / 2000 )
        parts_of_adjacency_a_list = NULL
        for(  current_table_number_in_sqlite_database  in  1:total_table_number_in_sqlite_database  ){
          start_column_number = ( current_table_number_in_sqlite_database - 1 ) * 2000 + 1
          end_column_number = start_column_number + 1999
          if( end_column_number >  total_column_number_of_adjaceny ){
            end_column_number = total_column_number_of_adjaceny
          }
          parts_of_adjacency_a_list  =   append (  parts_of_adjacency_a_list  ,   list (  adjacency_matrix[  , start_column_number:end_column_number ]  )     )
          gc()
        }

        # free the memory
        rm( adjacency_matrix )
        gc()
      }
      
      # write many data frames into SQLite database files
      {
        cores_cluster = makeCluster( total_number_of_cores )
        registerDoParallel( cores_cluster )
        result = foreach( current_table_number_in_sqlite_database  =  1:total_table_number_in_sqlite_database
                          , an_dataframe_of_parts_of_adjacency_a_list = parts_of_adjacency_a_list  
                          , .combine=c 
                          , .packages = c("DBI","RSQLite")  
                          ) %dopar%  
                            {
                              current_table_name_also_the_database_name_in_sqlite = paste( "the_" ,
                                                                                           current_table_number_in_sqlite_database ,
                                                                                           "_part_of_adjaceny_table_in_sqlite_database" ,
                                                                                           sep="")
                              connecting_to_sqlite_database  =  dbConnect(  RSQLite::SQLite( ) ,
                                                                            dbname = paste("/dev/shm/" ,
                                                                                           current_table_name_also_the_database_name_in_sqlite ,
                                                                                           sep="") )
                              dbSendQuery(   connecting_to_sqlite_database   ,   "PRAGMA  synchronous = OFF ;"  )
                              dbWriteTable(  connecting_to_sqlite_database  ,
                                             current_table_name_also_the_database_name_in_sqlite  , 
                                             an_dataframe_of_parts_of_adjacency_a_list )
                              return( TRUE )
                            }
        stopCluster( cores_cluster )
        
        if( sum( result ) == length( result ) )
        {
          cat("  all databases and tables used to save adjacency matrix have been created\n  ")
        }else{
          cat( "  some processes have errors\n ")
        }
        
        rm( parts_of_adjacency_a_list )
        gc()
      }
    }
    end_time_write_adjacency_to_sqlite  =  Sys.time( )
    cat( "\nWriting adjacency matrix to sqlite has finished. Time consumption： " ,  difftime(end_time_write_adjacency_to_sqlite , start_time_write_adjacency_to_sqlite , units="secs") , " seconds\n\n" )
    
    # write a table named moving_adjacency_from_R_to_sqlite_is_success_or_not into a SQLite database file
    {
      moving_adjacency_from_R_to_sqlite_is_success_or_not = data.frame( moving_adjacency_from_R_to_sqlite_is_success_or_not = c( 1 ) )
      dbWriteTable(  connecting_to_sqlite_database  , 
                     "moving_adjacency_from_R_to_sqlite_is_success_or_not"  ,  
                     moving_adjacency_from_R_to_sqlite_is_success_or_not  )
    }
  }
  
  # wait the calculating of TOM in C++
  {
    cat("R is waiting for TOM in sqlite from C++\n")
    TOM_is_in_sqlite_or_not <<- data.frame( c(0) )
    repeat {
      tryCatch(
        {
          TOM_is_in_sqlite_or_not   <<-    dbReadTable(   connecting_to_sqlite_database   ,    "TOM_is_in_sqlite_or_not"   )
        },
        warning = function(w){},
        error = function(e){},
        finally = {}
      )
      if ( TOM_is_in_sqlite_or_not[[1,1]] == 1 ) { 
        cat( "TOM is already in sqlite from C++\n" )
        break
      }else{
        Sys.sleep ( 3 ) 
      }
    }
  }
  
  # read the Topological Overlap Matrix (TOM) from SQLite database files
  {
    start_time_read_TOM_from_sqlite  =  Sys.time( )
    total_database_file_also_table_number_saving_TOM = ceiling( total_column_number_of_adjaceny / 2000 )
    cores_cluster = makeCluster( total_number_of_cores )
    registerDoParallel( cores_cluster )
    TOM  =   foreach( current_table_number_in_sqlite_database  =  1:total_database_file_also_table_number_saving_TOM
                      , .combine=cbind 
                      , .packages = c("DBI","RSQLite")  
                    ) %dopar%  
                      {
                        current_table_name_also_the_database_name_in_sqlite = paste( "the_" ,
                                                                                     current_table_number_in_sqlite_database ,
                                                                                     "_part_of_TOM_table_in_sqlite_database" ,
                                                                                     sep="")
                        connecting_to_an_database_file_of_current_process  =  dbConnect(  RSQLite::SQLite( ) ,
                                                                                          dbname = paste("/dev/shm/" ,
                                                                                                         current_table_name_also_the_database_name_in_sqlite ,
                                                                                                         sep="") )
                        dbSendQuery(   connecting_to_an_database_file_of_current_process   ,   "PRAGMA  synchronous = OFF ;"  )
                        
                        SQL_command_used_to_get_current_table_in_current_database_file = paste( "SELECT  *  FROM  " ,
                                                                                                current_table_name_also_the_database_name_in_sqlite,
                                                                                                sep="")
                        result_of_a_part_of_TOM = dbSendQuery(   connecting_to_an_database_file_of_current_process  , SQL_command_used_to_get_current_table_in_current_database_file )
                        a_part_of_TOM = dbFetch(   result_of_a_part_of_TOM   ) 

                        sql_string_delete_table = paste( "DROP TABLE " ,
                                                         current_table_name_also_the_database_name_in_sqlite ,
                                                         sep="")
                        dbSendQuery( connecting_to_an_database_file_of_current_process , sql_string_delete_table)
                        dbDisconnect(  connecting_to_an_database_file_of_current_process  )
                        gc()
                        
                        return( a_part_of_TOM )
                      }
    stopCluster( cores_cluster )
    
    TOM = as.matrix(TOM)
    gc()
    
    end_time_read_TOM_from_sqlite  =  Sys.time( )
    cat( "\nReading TOM matrix to R from sqlite has finished. Time consumption： " ,  difftime(end_time_read_TOM_from_sqlite , start_time_read_TOM_from_sqlite , units="secs") , " seconds\n\n" )
    
  }
  
  # delete the tables in a SQLite database file, and disconnect from the database
  table_names_vector = dbListTables(  connecting_to_sqlite_database  )
  for( current_table_name  in  table_names_vector ){
    sql_string_delete_table = paste( "DROP TABLE " ,
                                     current_table_name ,
                                     sep="")
    dbSendQuery( connecting_to_sqlite_database , sql_string_delete_table)
  }
  dbDisconnect(  connecting_to_sqlite_database  )
  gc()
  
  end_time_of_whole_TOM_calculation = Sys.time( )
  cat("\nUsing C++ OpenMP SQLite to calculate TOM from adjacency has been successful. Time consumption： " , difftime(end_time_of_whole_TOM_calculation , start_time_of_whole_TOM_calculation , units="mins") , " mins\n\n" )
}