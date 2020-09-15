#include "mld.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include<assert.h>

/*Application Structures*/

typedef struct emp_ {

    char emp_name[30];
    unsigned int emp_id;
    unsigned int age;
    struct emp_ *mgr;
    float salary;
    int *p;
} emp_t;

typedef struct student_{

    char stud_name[32];
    unsigned int rollno;
    unsigned int age;
    float aggregate;
    struct student_ *best_colleage;
} student_t;

int
main(int argc, char **argv){

    /*Step 1 : Initialize a new structure database */
    struct_db_t *struct_db = calloc(1, sizeof(struct_db_t));
    mld_init_primitive_data_types_support(struct_db);
        
    /*Step 2 : Create structure record for structure emp_t*/
    static field_info_t emp_fields[] = {
        FIELD_INFO(emp_t, emp_name, CHAR,    0),
        FIELD_INFO(emp_t, emp_id,   UINT32,  0),
        FIELD_INFO(emp_t, age,      UINT32,  0),
        FIELD_INFO(emp_t, mgr,      OBJ_PTR, emp_t),
        FIELD_INFO(emp_t, salary,   FLOAT, 0),
        FIELD_INFO(emp_t, p, OBJ_PTR, 0)
    };
    /*Step 3 : Register the structure in structure database*/
    REG_STRUCT(struct_db, emp_t, emp_fields);

    static field_info_t stud_fiels[] = {
        FIELD_INFO(student_t, stud_name, CHAR, 0),
        FIELD_INFO(student_t, rollno,    UINT32, 0),
        FIELD_INFO(student_t, age,       UINT32, 0),
        FIELD_INFO(student_t, aggregate, FLOAT, 0),
        FIELD_INFO(student_t, best_colleage, OBJ_PTR, student_t)
    };
    REG_STRUCT(struct_db, student_t, stud_fiels);

    /*Step 4 : Verify the correctness of structure database*/
    print_structure_db(struct_db);
    if(struct_db_look_up(struct_db,"student_t")){
        printf("struct found\n");
    }


    /*Working with object database*/
    /*Step 1 : Initialize a new Object database */
    object_db_t *object_db = calloc(1, sizeof(object_db_t));
    object_db->struct_db = struct_db;
    
    /*Step 2 : Create some sample objects, equivalent to standard 
     * calloc(1, sizeof(student_t))*/
    student_t *abhishek = xcalloc(object_db, "student_t", 1);
    strncpy(abhishek->stud_name, "abhishek", strlen("abhishek"));
    mld_set_dynamic_object_as_root(object_db, abhishek);

    student_t *shivani = xcalloc(object_db, "student_t", 1);
    strncpy(shivani->stud_name, "shivani", strlen("shivani"));
    abhishek->best_colleage = shivani;

    emp_t *joseph = xcalloc(object_db, "emp_t", 2);
    mld_set_dynamic_object_as_root(object_db, joseph);
    //int support not added 
    joseph->p = xcalloc(object_db, "int", 1);
    
    student_t *stud = xcalloc(object_db, "student_t", 1);
    strncpy(stud->stud_name, "stud", strlen("stud"));
    stud->rollno = 345;
    stud->age = 31;
    stud->aggregate = 86.8;
    stud->best_colleage = NULL;
    //abhishek->best_colleage = stud;
    print_object_db(object_db);
    run_mld_algorithms(object_db);
    printf("Leaked Objects : \n");
    report_leaked_objects(object_db);
    // xfree(object_db,stud);
    // print_object_db(object_db);
    // xfree(object_db,abhishek);
    // print_object_db(object_db);
    // xfree(object_db,shivani);
    // print_object_db(object_db);
    // xfree(object_db,joseph);
    // print_object_db(object_db);
    
    return 0;
}