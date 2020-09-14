#ifndef __MLD__
#define __MLD__
#include<stdint.h>

#define MAX_STRUCTURE_NAME_SIZE 128
#define MAX_FIELD_NAME_SIZE 128

typedef enum {
    UINT8,
    UINT32,
    INT32,
    CHAR,
    OBJ_PTR,
    FLOAT,
    DOUBLE,
    OBJ_STRUCT
} data_type_t;

#define OFFSETOF(struct_name,field_name)                            \
    (intptr_t)&(((struct_name*)0)->field_name)

#define FIELD_SIZE(struct_name,field_name)                          \
    sizeof(((struct_name*)0)->field_name)

#define FIELD_INFO(struct_name,field_name,dtype,nested_struct_name) \
    {#field_name,dtype,OFFSETOF(struct_name,field_name),            \
    FIELD_SIZE(struct_name,field_name),#nested_struct_name}

#define REG_STRUCT(struct_db,str_name,field_array)                  \
    {                                                               \
    struct_db_rec_t* rec = calloc(1,sizeof(struct_db_rec_t));       \
    strncpy(rec->struct_name,#str_name,MAX_STRUCTURE_NAME_SIZE);    \
    rec->ds_size = sizeof(str_name);                                \
    rec->n_fields = sizeof(field_array)/sizeof(field_info_t);       \
    rec->fields = field_array;                                      \
    if(add_structure_to_struct_db(struct_db,rec) != 0){             \
        printf("Error!Could not add the structure to the db\n");    \
        assert(0);                                                  \
    }}

typedef struct  _struct_db_rec_t_ struct_db_rec_t;

/* Structure to store the info of fields of the structures*/
typedef  struct field_info{
    char fname[MAX_FIELD_NAME_SIZE];
    data_type_t dtype;
    unsigned int offset;
    unsigned int size;
    //below field is only for obj_ptr and obj_struct
    char nested_str_name[MAX_STRUCTURE_NAME_SIZE];
}field_info_t;

//Structure to store the structure record
struct _struct_db_rec_t_{
    struct_db_rec_t *next; //pointer to the next structure in the linked list
    char struct_name[MAX_STRUCTURE_NAME_SIZE];//key
    unsigned int ds_size;   //size of the structure
    unsigned int n_fields;//no of fields
    field_info_t* fields; //pointer to the fields
};

//head of the linkedlist representing the structure db
typedef struct _struct_db_{
    struct_db_rec_t* head;
    unsigned int count;
}struct_db_t;

//Object db structures
typedef struct _object_db_rec_{
    struct _object_db_rec_* next;
    void* ptr;          //Key,i.e pointer to the object created
    unsigned int units;
    struct_db_rec_t* struct_rec;
}object_db_rec_t;

typedef struct _object_db_{
    struct_db_t* struct_db;
    object_db_rec_t* head;
    unsigned int count;
}object_db_t;

//Add structure to the database
int add_structure_to_struct_db(struct_db_t* struct_db,struct_db_rec_t* struct_rec);
//Printing functions
void print_structure_rec(struct_db_rec_t* struct_rec);
void print_structure_db(struct_db_t* struct_db);
void print_object_rec(object_db_rec_t* object_db_rec,int i);
void print_object_db(object_db_t* object_db);
//lookup by name
struct_db_rec_t* struct_db_look_up(struct_db_t *struct_db, char *struct_name);


void* xcalloc(object_db_t* object_db,char* struct_name,int units);


#endif