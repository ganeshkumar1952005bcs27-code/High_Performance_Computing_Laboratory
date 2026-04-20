#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define NAME_LEN 20

typedef struct {
    char name[NAME_LEN];
    int roll;
    int marks;
    char grade;
} Student;

char calculate_grade(int marks) {
    if (marks >= 90) return 'A';
    else if (marks >= 75) return 'B';
    else if (marks >= 60) return 'C';
    else return 'D';
}

int main() {
    int rank, size;
    int n;
    Student *students = NULL;
    Student *sub_students;
    int chunk;
    double start_time,end_time,exec_time;
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        FILE *fp = fopen("students.txt", "r");
        if (!fp) {
            printf("Error opening input file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fscanf(fp, "%d", &n);
        students = (Student *)malloc(n * sizeof(Student));

        for (int i = 0; i < n; i++) {
            fscanf(fp, "%s %d %d",
                   students[i].name,
                   &students[i].roll,
                   &students[i].marks);
            students[i].grade = ' ';
        }
        fclose(fp);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    chunk = n / size;
    sub_students = (Student *)malloc(chunk * sizeof(Student));

    MPI_Datatype MPI_STUDENT;
    int blocklengths[] = {NAME_LEN, 1, 1, 1};
    MPI_Aint name_addr,roll_addr,mark_addr,grade_addr;
    MPI_Aint offsets[4] = {0};
    MPI_Get_address(students->name,&name_addr);
    MPI_Get_address(&students->roll,&roll_addr);
    MPI_Get_address(&students->marks,&mark_addr);
    MPI_Get_address(&students->grade,&grade_addr);
    offsets[1]=roll_addr-name_addr;
    offsets[2]=mark_addr-name_addr;
    offsets[3]=grade_addr-name_addr;
    MPI_Datatype types[] = {MPI_CHAR, MPI_INT, MPI_INT, MPI_CHAR};

    MPI_Type_create_struct(4, blocklengths, offsets, types, &MPI_STUDENT);
    MPI_Type_commit(&MPI_STUDENT);
    MPI_Scatter(students, chunk, MPI_STUDENT,
                sub_students, chunk, MPI_STUDENT,
                0, MPI_COMM_WORLD);
    start_time=MPI_Wtime();
    for (int i = 0; i < chunk; i++) {
        sub_students[i].grade = calculate_grade(sub_students[i].marks);
        printf("Process %d :\tName:%s \tRoll:%d\tMark:%d\tGrade Calculated:%c\n",rank,sub_students[i].name,sub_students[i].roll,sub_students[i].marks,sub_students[i].grade);
    }
    end_time=MPI_Wtime();
    exec_time=end_time-start_time;
    printf("Process %d execution time : %f milliseconds\n",rank,exec_time*1000);

    MPI_Gather(sub_students, chunk, MPI_STUDENT,
               students, chunk, MPI_STUDENT,
               0, MPI_COMM_WORLD);
    if (rank == 0) {
        FILE *out = fopen("grades.txt", "w");
        fprintf(out,"Name\tRoll\tMarks\tGrade\n");

        for (int i = 0; i < n; i++) {
            fprintf(out, "%s\t%d\t%d\t%c\n",
                    students[i].name,
                    students[i].roll,
                    students[i].marks,
                    students[i].grade);
        }
        fclose(out);
        printf("Grades written to grades.txt\n");
    }

    free(sub_students);
    if (rank == 0)
        free(students);

    MPI_Type_free(&MPI_STUDENT);
    MPI_Finalize();
    return 0;
}