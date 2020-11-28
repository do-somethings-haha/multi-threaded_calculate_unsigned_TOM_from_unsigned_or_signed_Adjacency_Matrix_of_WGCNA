# include <string>
# include <cstring>
# include <iostream>
# include <fstream>
# include <sstream>
# include <cstdlib>
# include <vector>
# include <omp.h>
# include <cmath>
# include <sqlite3.h>
# include <iomanip>
# include <unistd.h>
using namespace std;

int main()
{
    // Connect to SQLite
    sqlite3  *  database_pointer ;
    int success_or_fail   =   sqlite3_open(   "/dev/shm/sqlite_database_file_in_memory"  ,  & database_pointer   );
    if ( success_or_fail  ==  SQLITE_OK )
    {
        cout<<"It's successful to connect to SQLite database"<<endl<<endl;
        sqlite3_exec(  database_pointer  ,  "PRAGMA  synchronous = OFF ;" , 0  , 0 , 0 ) ;
    }
    else
    {
        cerr<<"It's failed to connect to SQLite database"<<endl<<endl;
    }

    // Determine whether R language has written the Adjacency Matrix into database files
    cout<<"Waiting for moving adjacency from R to sqlite"<<endl;
    double moving_adjacency_from_R_to_sqlite_is_success_1_or_not_0 = 0.0;
    char ** results_moving_adjacency_from_R_to_sqlite_is_success_or_not;
    sqlite3_stmt  *  stmt_pointer_1 ;
    try
       {
        sqlite3_prepare_v2(  database_pointer  ,   "SELECT * FROM moving_adjacency_from_R_to_sqlite_is_success_or_not"  ,  strlen("SELECT * FROM moving_adjacency_from_R_to_sqlite_is_success_or_not")  ,   &stmt_pointer_1  ,  NULL  );
        sqlite3_step(  stmt_pointer_1  );
        moving_adjacency_from_R_to_sqlite_is_success_1_or_not_0  =  sqlite3_column_double(  stmt_pointer_1  ,  0  );
            if( moving_adjacency_from_R_to_sqlite_is_success_1_or_not_0 == 0 )
           {
               throw  moving_adjacency_from_R_to_sqlite_is_success_1_or_not_0 ;
           }
       }
    catch ( ... ) { cerr<<"   SQLite database has not have the adjacency matrix"<<endl; }
    while( moving_adjacency_from_R_to_sqlite_is_success_1_or_not_0 == 0 )
    {
        sleep(3);
        cout<<"   C++ is waiting for adjacency in sqlite from R"<<endl;
        try
           {
                sqlite3_prepare_v2(  database_pointer  ,   "SELECT * FROM moving_adjacency_from_R_to_sqlite_is_success_or_not"  ,  strlen("SELECT * FROM moving_adjacency_from_R_to_sqlite_is_success_or_not")  ,   &stmt_pointer_1  ,  NULL  );
                sqlite3_step(  stmt_pointer_1  );
                moving_adjacency_from_R_to_sqlite_is_success_1_or_not_0  =  sqlite3_column_double(  stmt_pointer_1  ,  0  );
           }
        catch ( ... ) { }
    }
    sqlite3_free_table( results_moving_adjacency_from_R_to_sqlite_is_success_or_not );
    cout<<endl<<"Moving adjacency from R to sqlite is success"<<endl<<endl;


    // Get total_gene_number
    int total_gene_number  ;
    char ** results_total_gene_number;
    sqlite3_get_table(  database_pointer  ,  "SELECT  * FROM  total_gene_number"   ,   &results_total_gene_number   ,    NULL   ,   NULL  ,   NULL  );
    stringstream  string_stream_object ;
    string_stream_object << results_total_gene_number[1] ;
    string_stream_object >> total_gene_number ;
    string_stream_object.str("");
    string_stream_object.clear( ) ;
    sqlite3_free_table(  results_total_gene_number  );

    // Figure out the number of tables used to store the Ajacency Matrix
    int total_table_number_used_to_save_adjaceny = ceil(  ( (double)total_gene_number) /2000 );
    cout<<"total_gene_number:  "<<total_gene_number<<endl;
    cout<<"total_table_number_used_to_save_adjaceny:  "<<total_table_number_used_to_save_adjaceny<<endl<<endl;


    // Create a two-dimensional array used to store the Ajacency Matrix
    double ** array_2_dimension_adjacency = new double * [ total_gene_number ];
    for( int i = 0 ; i<= total_gene_number-1 ; i++)
    {
        array_2_dimension_adjacency[i] = new double[total_gene_number];
    }
    cout<<"creating a blank array_2_dimension_adjacency was successful."<<endl<<"Now, read adjacency matrix from sqlite into C++ array_2_dimension_adjacency"<<endl;

    // Read the Ajacency Matrix into C++ from SQLite
    long int start_time_of_read_adjacency_from_sqlite = omp_get_wtime();
    for( int current_table_number_used_to_save_adjaceny = 1 ;  current_table_number_used_to_save_adjaceny <= total_table_number_used_to_save_adjaceny ; current_table_number_used_to_save_adjaceny++ )
    {
         stringstream string_stream_object;
         int total_column_number_of_current_table = 0 ;
         string current_database_also_table_name_saving_a_part_of_adjacency;
         string sql_command_read_current_database_table ;
         string sql_command_delete_current_database_table;

         // Get current database file name also the table name
         string_stream_object  <<  "the_"  << current_table_number_used_to_save_adjaceny << "_part_of_adjaceny_table_in_sqlite_database";
         current_database_also_table_name_saving_a_part_of_adjacency = string_stream_object.str();
         string_stream_object.str("");
         string_stream_object.clear();
         cout<<"  copy the "<<current_table_number_used_to_save_adjaceny<<" table in sqlite to array_2_dimension_adjacency in C++"<<endl;
         cout<<"     current table name:  "<<current_database_also_table_name_saving_a_part_of_adjacency<<endl;

         // Figure out the total column number of current table
         if( current_table_number_used_to_save_adjaceny < total_table_number_used_to_save_adjaceny )
         {
             total_column_number_of_current_table = 2000;
         }
         else if( current_table_number_used_to_save_adjaceny == total_table_number_used_to_save_adjaceny )
         {
             total_column_number_of_current_table = total_gene_number - ( current_table_number_used_to_save_adjaceny - 1 ) * 2000 ;
         }

         // Connect to current database file and read the data
         sqlite3  *  database_pointer_of_current_thread ;
         string current_database_file_path = "/dev/shm/" + current_database_also_table_name_saving_a_part_of_adjacency;
         sqlite3_open(   current_database_file_path.c_str()  ,  & database_pointer_of_current_thread   );
         sql_command_read_current_database_table = "SELECT  *  from  " + current_database_also_table_name_saving_a_part_of_adjacency;
         sqlite3_stmt  *  stmt_pointer;
         sqlite3_prepare_v2(  database_pointer_of_current_thread  ,   sql_command_read_current_database_table.c_str() ,  sql_command_read_current_database_table.size() ,   &stmt_pointer  ,  NULL  );
         for( int data_row_index = 0 ; data_row_index <=  total_gene_number - 1 ; data_row_index++ )
         {
             sqlite3_step(  stmt_pointer  );
             for( int column_index = 0 ; column_index <= total_column_number_of_current_table - 1 ; column_index++ )
             {
                 double the_value_of_current_table_cell  =  sqlite3_column_double(  stmt_pointer  ,  column_index  );
                 array_2_dimension_adjacency[ data_row_index ] [ column_index +  (current_table_number_used_to_save_adjaceny-1)*2000 ] = the_value_of_current_table_cell;
             }
         }
        sqlite3_finalize( stmt_pointer );

         // Delete current table and disconnect to current database file
         sql_command_delete_current_database_table = "DROP  TABLE " + current_database_also_table_name_saving_a_part_of_adjacency;
         sqlite3_exec(  database_pointer_of_current_thread  ,  sql_command_delete_current_database_table.c_str()  ,  NULL  ,  NULL  , NULL  );
         cout<<"     current sqlite table has been imported to C++ and deleted"<<endl;
         sqlite3_close(  database_pointer_of_current_thread  );
    }
    long int end_time_of_read_adjacency_from_sqlite = omp_get_wtime();
    cout<<endl<<"array_2_dimension_adjacency has had whole data of adjacency. Time consumption: "<<end_time_of_read_adjacency_from_sqlite-start_time_of_read_adjacency_from_sqlite<<" seconds"<<endl<<endl;

    // Calculate TOM
    long int start_time_of_calculate_TOM_from_adjacency = omp_get_wtime();
    // Create a blank TOM
    double ** array_2_dimension_TOM = new double * [ total_gene_number ];
    for( int i = 0 ; i <= total_gene_number-1 ; i++)
    {
        array_2_dimension_TOM[ i ]  =  new double[ total_gene_number ];
    }
    cout<<"creating a blank TOM has finished. Begin to calculate the TOM."<<endl<<"   Please wait"<<endl<<endl;

    // Create a one-dimensional array, and calculate the intermediate values of the denominator
    double * sums_of_each_row_in_an_array = new double [ total_gene_number ];
    #pragma omp parallel proc_bind( spread )
    {
        #pragma omp for schedule( static )
        for( int current_row_index = 0 ; current_row_index <= total_gene_number - 1 ; current_row_index++  )
        {
            sums_of_each_row_in_an_array[ current_row_index ] = 0;
            for( int current_column_index = 0 ; current_column_index <= total_gene_number - 1 ; current_column_index++  )
            {
                sums_of_each_row_in_an_array[ current_row_index ] = sums_of_each_row_in_an_array[ current_row_index ] + array_2_dimension_adjacency[ current_row_index ][ current_column_index ];
            }

        }
    }

    // Calculate the intermediate values of the numerator
    double * multiply_and_sum_of_combination_of_i_j_Change_triangular_matrix_to_an_array = new double [ (total_gene_number*(total_gene_number-1))/2 ];
    #pragma omp parallel proc_bind( spread )
    {
        #pragma omp single
        {
            int how_many_table_cells_have_been_computed_in_triangular_matrix = 0;
            for( int current_row_index = 0 ; current_row_index <= total_gene_number - 2 ; current_row_index++ )
            {
                int how_many_table_cells_should_be_computed_in_current_row = 0 ;
                if( current_row_index == 0 )
                {
                    how_many_table_cells_have_been_computed_in_triangular_matrix = 0;
                }
                else
                {
                    how_many_table_cells_should_be_computed_in_current_row = total_gene_number - (current_row_index-1) - 1;
                    how_many_table_cells_have_been_computed_in_triangular_matrix = how_many_table_cells_have_been_computed_in_triangular_matrix + how_many_table_cells_should_be_computed_in_current_row;
                }
                #pragma omp task
                {
                    for( int a_later_row_index_behind_current_row = current_row_index + 1 ; a_later_row_index_behind_current_row <= total_gene_number - 1 ; a_later_row_index_behind_current_row++  )
                    {
                        double current_i_j_combination_value = 0.0;
                        for( int current_column_index = 0 ; current_column_index <= total_gene_number-1 ; current_column_index++)
                        {
                            current_i_j_combination_value  =  current_i_j_combination_value  +  array_2_dimension_adjacency[ current_row_index ] [ current_column_index ]  *  array_2_dimension_adjacency[ a_later_row_index_behind_current_row ][ current_column_index ] ;
                        }
                        int index_of_current_i_j_combination_value = how_many_table_cells_have_been_computed_in_triangular_matrix - 1  +  ( a_later_row_index_behind_current_row - current_row_index );
                        multiply_and_sum_of_combination_of_i_j_Change_triangular_matrix_to_an_array[ index_of_current_i_j_combination_value ] = current_i_j_combination_value;
                    }
                }
            }
        }
    }

    // Calculate the final results
    #pragma omp parallel proc_bind( spread )
    {
        #pragma omp single
        {
            int how_many_table_cells_have_been_computed_in_triangular_matrix = 0;
            for( int current_row_index = 0 ; current_row_index <= total_gene_number - 2 ; current_row_index++ )
            {
                int how_many_table_cells_should_be_computed_in_current_row = 0 ;
                if( current_row_index == 0 )
                {
                    how_many_table_cells_have_been_computed_in_triangular_matrix = 0;
                }
                else
                {
                    how_many_table_cells_should_be_computed_in_current_row = total_gene_number - (current_row_index-1) - 1;
                    how_many_table_cells_have_been_computed_in_triangular_matrix = how_many_table_cells_have_been_computed_in_triangular_matrix + how_many_table_cells_should_be_computed_in_current_row;
                }
                #pragma omp task
                {
                    for( int current_column_index = current_row_index + 1 ; current_column_index <= total_gene_number - 1 ; current_column_index++  )
                    {
                        int index_of_current_i_j_combination_value = how_many_table_cells_have_been_computed_in_triangular_matrix - 1  +  ( current_column_index - current_row_index );
                        double xigema_a_iu_a_ju =  multiply_and_sum_of_combination_of_i_j_Change_triangular_matrix_to_an_array[ index_of_current_i_j_combination_value ];
                        double xigema_a_iu = sums_of_each_row_in_an_array[ current_row_index ];
                        double xigema_a_ju = sums_of_each_row_in_an_array[ current_column_index ];
                        double a_ij = array_2_dimension_adjacency[ current_row_index ][ current_column_index ];
                        double value_of_TOM_current_row_column = ( xigema_a_iu_a_ju - a_ij ) / ( ( ( xigema_a_iu + xigema_a_ju )/2 ) - a_ij ) ;
                        array_2_dimension_TOM[ current_row_index ][ current_column_index ] = value_of_TOM_current_row_column;
                        array_2_dimension_TOM[ current_column_index ][ current_row_index ] = value_of_TOM_current_row_column;
                    }
                }
            }
        }
    }

    // Assign the slash in the middle of the TOM matrix to 1
    #pragma omp parallel proc_bind( spread )
    {
        #pragma omp for schedule( static )
        for( int row_index = 0 ; row_index <= total_gene_number -1 ; row_index++  )
        {
            array_2_dimension_TOM[ row_index ][ row_index ] = 1.0;
        }
    }

    long int end_time_of_calculate_TOM_from_adjacency = omp_get_wtime();
    cout<<endl<<"Time consumption of calculating TOM from adjacency: "<<end_time_of_calculate_TOM_from_adjacency - start_time_of_calculate_TOM_from_adjacency<<" seconds , "<<( (double)(end_time_of_calculate_TOM_from_adjacency - start_time_of_calculate_TOM_from_adjacency) )/60<<" minutes "<<endl<<endl;

    // Delete the Adjacency Matrix and the arrays used to store intermediate values
    for(  int row_index_of_array_2_dimension_adjacency = 0  ;  row_index_of_array_2_dimension_adjacency <= total_gene_number - 1  ;  row_index_of_array_2_dimension_adjacency++  )
    {
        delete [ ]   array_2_dimension_adjacency[ row_index_of_array_2_dimension_adjacency ];
    }
    delete [ ] array_2_dimension_adjacency ;
    delete [ ] sums_of_each_row_in_an_array ;
    delete [ ] multiply_and_sum_of_combination_of_i_j_Change_triangular_matrix_to_an_array ;

    // Write TOM into SQLite database files
     cout<<"write TOM into sqlite database"<<endl<<endl;
     long int start_time_of_writing_TOM_into_sqlite = omp_get_wtime();
     #pragma omp parallel proc_bind( spread )
     {
         int total_database_file_also_table_number_used_to_save_TOM = total_table_number_used_to_save_adjaceny;
         stringstream string_stream_object;
         int start_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM = 0;
         int end_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM = 0;
         string current_database_file_also_table_name_saving_a_part_of_TOM;
         string SQL_command_used_to_create_a_table_to_save_current_part_of_TOM ;
         string SQL_command_used_to_write_data_into_current_table_that_saves_a_part_of_TOM ;
         string SQL_command_delete_current_database_table;

         #pragma omp for
         for( int current_database_file_also_table_number_used_to_save_TOM = 1 ; current_database_file_also_table_number_used_to_save_TOM <= total_database_file_also_table_number_used_to_save_TOM ; current_database_file_also_table_number_used_to_save_TOM++ )
         {
            // Get current database file name also the table name
            string_stream_object  <<  "the_"  << current_database_file_also_table_number_used_to_save_TOM << "_part_of_TOM_table_in_sqlite_database";
            current_database_file_also_table_name_saving_a_part_of_TOM = string_stream_object.str();
            string_stream_object.str("");
            string_stream_object.clear();

            // Connect to current database file
            sqlite3  *  database_pointer_of_current_thread ;
            string current_database_file_path = "/dev/shm/" + current_database_file_also_table_name_saving_a_part_of_TOM;
            sqlite3_open(   current_database_file_path.c_str()  ,  & database_pointer_of_current_thread   );

            // Create a table in current database file
            start_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM = ( current_database_file_also_table_number_used_to_save_TOM - 1 ) * 2000 + 1 ;
            end_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM = start_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM + 1999 ;
            if( end_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM > total_gene_number )
            {
                end_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM = total_gene_number ;
            }
            string_stream_object << "CREATE"<<" "<<"TABLE"<<" "<< current_database_file_also_table_name_saving_a_part_of_TOM << "(";
            for( int current_gene_number = start_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM ; current_gene_number <=  end_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM ; current_gene_number++ )
            {
                string_stream_object << "V" <<current_gene_number << " REAL" ;
                if( current_gene_number != end_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM )
                {
                 string_stream_object << ",";
                }
            }
            string_stream_object << ");";
            SQL_command_used_to_create_a_table_to_save_current_part_of_TOM = string_stream_object.str();
            string_stream_object.str("");
            string_stream_object.clear();
            sqlite3_exec(  database_pointer_of_current_thread  ,  SQL_command_used_to_create_a_table_to_save_current_part_of_TOM.c_str()  ,  NULL  ,  NULL  , NULL  );

            // Write a part of TOM into current table
            sqlite3_stmt  * stmt_pointer;
            string_stream_object << "INSERT  INTO  "<< current_database_file_also_table_name_saving_a_part_of_TOM << "(";
             for( int current_gene_number = start_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM ; current_gene_number <=  end_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM ; current_gene_number++ )
             {
                 string_stream_object << "V" <<current_gene_number;
                 if( current_gene_number != end_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM )
                 {
                     string_stream_object << ",";
                 }
             }
             string_stream_object << " ) VALUES ( ";
             for( int current_gene_number = start_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM ; current_gene_number <=  end_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM ; current_gene_number++ )
             {
                 string_stream_object << "?";
                 if( current_gene_number != end_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM )
                 {
                     string_stream_object << ",";
                 }
             }
            string_stream_object << " );";
            SQL_command_used_to_write_data_into_current_table_that_saves_a_part_of_TOM = string_stream_object.str();
            string_stream_object.str("");
            string_stream_object.clear();
            sqlite3_prepare_v2(  database_pointer_of_current_thread  , SQL_command_used_to_write_data_into_current_table_that_saves_a_part_of_TOM.c_str() , -1 , &stmt_pointer , 0 );
            for(int row_index_of_current_part_of_TOM = 0 ; row_index_of_current_part_of_TOM <= total_gene_number -1  ; row_index_of_current_part_of_TOM++)
            {
                 sqlite3_reset(  stmt_pointer  );
                 for( int  column_number_of_current_part_of_TOM =  start_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM ; column_number_of_current_part_of_TOM <= end_gene_number_in_current_database_file_also_table_saving_a_part_of_TOM ; column_number_of_current_part_of_TOM ++  )
                 {
                     int question_mark_numbe_in_current_SQL_command = column_number_of_current_part_of_TOM - ( current_database_file_also_table_number_used_to_save_TOM - 1 ) * 2000;
                     sqlite3_bind_double(  stmt_pointer , question_mark_numbe_in_current_SQL_command , array_2_dimension_TOM[row_index_of_current_part_of_TOM][column_number_of_current_part_of_TOM-1] );
                 }
                 sqlite3_step( stmt_pointer );
            }
            sqlite3_finalize( stmt_pointer );

            // Disconnect to current databse file
            sqlite3_close(  database_pointer_of_current_thread  );

         }
     }

    // Say to R language that it can read TOM
    sqlite3_exec(  database_pointer  ,  "CREATE   TABLE  TOM_is_in_sqlite_or_not ( TOM_is_in_sqlite_or_not  REAL );"  ,  NULL  ,  NULL  , NULL  );
    sqlite3_exec(  database_pointer  ,  "INSERT   INTO  TOM_is_in_sqlite_or_not ( TOM_is_in_sqlite_or_not ) VALUES ( 1 ); "  ,  NULL  ,  NULL  , NULL  );
    long int end_time_of_writing_TOM_into_sqlite = omp_get_wtime();
    cout<<"Writing TOM into sqlite has been successful. Time consumption of writing TOM into sqlite: "<<end_time_of_writing_TOM_into_sqlite - start_time_of_writing_TOM_into_sqlite<<" seconds , "<<( (double)(end_time_of_writing_TOM_into_sqlite - start_time_of_writing_TOM_into_sqlite) )/60<<" minutes "<<endl<<endl;

    // Disconnect to current databse file and delete the two-dimensional array used to store TOM
    sqlite3_close(  database_pointer  );
    for(  int row_index_of_array_2_dimension_TOM = 0  ;  row_index_of_array_2_dimension_TOM <= total_gene_number - 1  ;  row_index_of_array_2_dimension_TOM++  )
    {
        delete [ ]   array_2_dimension_TOM[ row_index_of_array_2_dimension_TOM ];
    }
    delete [ ] array_2_dimension_TOM ;

    return 0;
}
