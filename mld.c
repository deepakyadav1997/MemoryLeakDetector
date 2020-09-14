#include<stdio.h>
#include "mld.h"
#include "css.h"
#include<assert.h>
#include<string.h>
#include<stdlib.h>
#include<memory.h>

char *DATA_TYPE[] = {"UINT8", "UINT32", "INT32",
                     "CHAR", "OBJ_PTR", "VOID_PTR", "FLOAT",
                     "DOUBLE", "OBJ_STRUCT"};


int add_structure_to_struct_db(struct_db_t* struct_db,struct_db_rec_t* struct_rec){
    struct_rec->next = struct_db->head;
    struct_db->head = struct_rec;
    struct_db->count++; 
    return 0;
}
struct_db_rec_t* struct_db_look_up(struct_db_t *struct_db, char *struct_name){
    if(!struct_db)
        return NULL;
    struct_db_rec_t *current_record = struct_db->head;
    //printf("Total Structures: %d\n",struct_db->count);
    while(current_record){
        if(strcmp(current_record->struct_name,struct_name) == 0)
            return current_record;
        current_record = current_record->next;
    }
    return NULL;
}

//Printing functions
void print_structure_rec(struct_db_rec_t* struct_rec){
    if(!struct_rec) return;
        int j = 0;
    field_info_t *field = NULL;
    printf(ANSI_COLOR_BLUE "|------------------------------------------------------|\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_RED "| %-20s | size = %-8d | #flds = %-3d |\n" ANSI_COLOR_RESET, struct_rec->struct_name, struct_rec->ds_size, struct_rec->n_fields);
    printf(ANSI_COLOR_BLUE "|------------------------------------------------------|------------------------------------------------------------------------------------------|\n" ANSI_COLOR_RESET);
    for(j = 0; j < struct_rec->n_fields; j++){
        field = &struct_rec->fields[j];
        printf("  %-20s |", "");
        printf("%-3d %-20s | dtype = %-15s | size = %-5d | offset = %-6d|  nstructname = %-20s  |\n",
                j, field->fname, DATA_TYPE[field->dtype], field->size, field->offset, field->nested_str_name);
        printf("  %-20s |", "");
        printf(ANSI_COLOR_BLUE "--------------------------------------------------------------------------------------------------------------------------|\n" ANSI_COLOR_RESET);
    }
}
void print_structure_db(struct_db_t* struct_db){
    if(!struct_db)
        return;
    struct_db_rec_t *current_record = struct_db->head;
    printf("Total Structures: %d\n",struct_db->count);
    while(current_record){
        print_structure_rec(current_record);
        current_record = current_record->next;
    }
}
void print_object_rec(object_db_rec_t *obj_rec, int i){
    if(!obj_rec) return;
    printf(ANSI_COLOR_MAGENTA "-----------------------------------------------------------------------------------------------------|\n"ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW "%-3d ptr = %-10p | next = %-10p | units = %-4d | struct_name = %-10s |\n"ANSI_COLOR_RESET, 
        i, obj_rec->ptr, obj_rec->next, obj_rec->units, obj_rec->struct_rec->struct_name); 
    printf(ANSI_COLOR_MAGENTA "-----------------------------------------------------------------------------------------------------|\n"ANSI_COLOR_RESET);
}

void print_object_db(object_db_t *object_db){
    object_db_rec_t *head = object_db->head;
    unsigned int i = 0;
    printf(ANSI_COLOR_CYAN "Printing OBJECT DATABASE\n");
    for(; head; head = head->next){
        print_object_rec(head, i++);
    }
}
static  object_db_rec_t* object_db_lookup(object_db_t* object_db,
                                            void* ptr
                                            ){
    object_db_rec_t* current_record =  object_db->head;
    while(current_record){
        if(current_record->ptr == ptr){
            return current_record;
        }
        current_record = current_record->next;
    }  
    return NULL;                              
}

static void add_object_to_db(object_db_t* object_db,
                             void* ptr,
                             int units,
                             struct_db_rec_t* struct_rec
                            ){
    //Check if the object exists                            
    object_db_rec_t* obj_rec = object_db_lookup(object_db,ptr);
    if(obj_rec != NULL){
        printf("Object already exists in the database\n");
        assert(0);
    } 
    //Initializng the new object record
    obj_rec = (object_db_rec_t*)calloc(1,sizeof(object_db_rec_t));  
    obj_rec->ptr = ptr;
    obj_rec->struct_rec = struct_rec;
    obj_rec->next =NULL;
    obj_rec->units = units;

    //Add the object to the object DB linkedlist
    obj_rec->next = object_db->head;
    object_db->head = obj_rec;
    object_db->count++;

}

void* xcalloc(object_db_t* object_db,
            char* struct_name,
            int units
            ){
    struct_db_rec_t* struct_rec = struct_db_look_up(object_db->struct_db,
                                                    struct_name);
    assert(struct_rec);
    void* ptr = calloc(units,struct_rec->ds_size);
    add_object_to_db(object_db,ptr,units,struct_rec);
    return ptr;
}