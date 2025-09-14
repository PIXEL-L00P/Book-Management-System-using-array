#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#ifdef _WIN32
    #define strcasecmp _stricmp   // Windows uses _stricmp
#else
    #include <strings.h>         // Linux/macOS use strcasecmp
#endif

/*strcmp is the main function. it is case sensitive, as it read chair and ChaiR as two different string.
  So we should use stricmp(for windows) or strcasecmp(for max or linux)
  To make it portable(work in any os) we should use those header.
  Since on windows the #define makes strcasecmp act as _stricmp .
  On Linux/macOS The strings.h header provides native strcasecmp.*/

struct book_info
{
    int book_id;
    char title[200];
    char author[200];
};

struct book_info *book = NULL;
int book_count = 0;
struct book_info **b; /*This is an array of pointers to books,
                        not an array of books themselves.
                        It will holds the address of book .
                        You can say it as pointer to a pointer*/

char filename[256] = ""; // Global variable to store the filename

void get_filename()
{
    char response[10];
    char new_filename[256];

    if (filename[0] != '\0') {
        // Ask if user wants to use a different file
        printf("Current file: %s\n", filename);
        while(1)
        {
            printf("Do you want to use a different file? (yes/no): ");
            fgets(response, sizeof(response), stdin);
            response[strcspn(response, "\n")] = '\0';

            if (strcasecmp(response, "no") == 0 || strcasecmp(response, "n") == 0)
            {
                printf("Using previous file: %s\n", filename);
                return; // Keep using the same file
            }
            else if (strcasecmp(response, "yes") == 0 || strcasecmp(response, "y") == 0)
            {
                break; // Continue to get new filename
            }
            else
            {
                printf("Please answer 'yes' or 'no'.\n");
            }
        }
    }

    // Get new filename
    while(1)
    {
        printf("Enter filename for saving/loading books (with extension .csv): ");
        fgets(new_filename, sizeof(new_filename), stdin);
        new_filename[strcspn(new_filename, "\n")] = '\0';

        if (new_filename[0] != '\0')
        {
            strcpy(filename, new_filename);
            printf("Using file: %s\n", filename);
            save_filename_to_config();
            break; // Exit the loop when valid filename is provided
        }
        else {
            printf("Error: Please enter a filename.\n");
        }
    }
}

void save_filename_to_config()
{
    FILE *config_file = fopen("library_config.txt", "w");
    if (config_file != NULL) {
        fprintf(config_file, "%s", filename);
        fclose(config_file);
        printf("Configuration saved to library_config.txt\n");
    } else {
        printf("Warning: Could not save configuration file\n");
    }
}

void load_filename_from_config()
{
    FILE *config_file = fopen("library_config.txt", "r");
    if (config_file != NULL)
    {
        if (fgets(filename, sizeof(filename), config_file) != NULL)
        {
            filename[strcspn(filename, "\n")] = '\0';
            printf("Loaded previous configuration: %s\n", filename);
        }
        fclose(config_file);
    }
    else
    {
        printf("No previous configuration found. Starting fresh.\n");
    }
}


void save_books_to_file()
{
    get_filename();


    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not create file '%s'.\n", filename);
        return;
    }

    // Write CSV header
    fprintf(file, "book_id,title,author\n");

    // Write each book's data
    for (int i = 0; i < book_count; i++) {
        fprintf(file, "%d,\"%s\",\"%s\"\n",
                book[i].book_id, book[i].title, book[i].author);
    }

    fclose(file);
    printf("Books saved successfully to '%s'!\n", filename);
}

void load_books_from_file()
{
    if (filename[0] == '\0')
    {
        return;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("No existing book data found in '%s'. Starting with empty library.\n", filename);
        return;
    }

    // Count lines in file (excluding header)
    int line_count = 0;
    char buffer[500];
    fgets(buffer, sizeof(buffer), file); // Skip header

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        line_count++;
    }

    if (line_count == 0) {
        printf("No books found in file '%s'.\n", filename);
        fclose(file);
        return;
    }

    // Reset file pointer
    rewind(file);
    fgets(buffer, sizeof(buffer), file); // Skip header again

    // Save current book count to append to existing books
    int original_count = book_count;

    // Reallocate memory to accommodate both existing and new books
    struct book_info *temp = (struct book_info*)realloc(book, (book_count + line_count) * sizeof(struct book_info));
    if (temp == NULL) {
        printf("Memory allocation failed.\n");
        fclose(file);
        return;
    }
    book = temp;

    // Read books from file and append to existing books
    int loaded_count = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL && loaded_count < line_count) {
        // Remove newline character
        buffer[strcspn(buffer, "\n")] = '\0';

        // Parse CSV line
        char *token;
        token = strtok(buffer, ",");
        if (token == NULL) continue;

        book[book_count + loaded_count].book_id = atoi(token);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        // Remove quotes if present
        if (token[0] == '"' && token[strlen(token)-1] == '"') {
            token[strlen(token)-1] = '\0';
            strcpy(book[book_count + loaded_count].title, token + 1);
        } else {
            strcpy(book[book_count + loaded_count].title, token);
        }

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        // Remove quotes if present
        if (token[0] == '"' && token[strlen(token)-1] == '"') {
            token[strlen(token)-1] = '\0';
            strcpy(book[book_count + loaded_count].author, token + 1);
        } else {
            strcpy(book[book_count + loaded_count].author, token);
        }

        loaded_count++;
    }

    book_count += loaded_count;
    fclose(file);
    printf("Successfully loaded %d books from '%s'! Total books: %d\n", loaded_count, filename, book_count);
}

int is_empty()
{
    if(book_count==0)
    {
        return 1;
    }

    return 0;
}

int verify_pos_integer(char string[200])
{
    int c;
    while(1)
        {
            printf("%s",string);
            if(scanf("%d",&c) != 1 || c <0)// return value of scanf
            {
                /* if the input is a valid integer it will return 1
                   if the input is not valid it will return 0/EOF; */
                printf("Invalid input! Please enter a positive integer number.\n");
                while (getchar() != '\n');//if a user give char input the while loop will run infinite times. this line prevent to do this.
            }

            else
                break;
        }

        return c;
}

void sorted_by_ID()
{
    int i, j;

    for(i=1; i<book_count; i++)
    {
        j = i-1;
        struct book_info key = book[i];

        while (j >= 0 && book[j].book_id > key.book_id)
        {
            book[j + 1] = book[j];
            j--;
        }
        book[j + 1] = key;
    }
}

int search_by_ID(int value)
{
    int first = 0, last = book_count-1, mid;

    while(first<=last)
    {
        mid = (first + last)/2;

        if(book[mid].book_id == value)
        {
            return mid;
        }

        else if(book[mid].book_id > value)
        {
            last = mid-1;
        }

        else
        {
            first = mid+1;
        }
    }
    return -1;
}

void sorted_by_title()// it will sort the memory address and store them in b.
{
    b = malloc(book_count * sizeof(struct book_info *));
    if (b==NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    // Point each entry to the original books
    for (int i = 0; i < book_count; i++) {
        b[i] = &book[i];
    }

    int i, j;
    /*here we will compare the titles
    and then we will sort the addresses according to their titles*/

    for (i = 1; i < book_count; i++)
    {
        struct book_info *key = b[i];
        j = i - 1;

        while (j >= 0 && strcasecmp(b[j]->title, key->title) > 0)
        {
            b[j + 1] = b[j];
            j--;
        }
        b[j + 1] = key;
    }
}
//     (mid>char) <---------d,e,----------mid(g)---------k,l----------> (mid<char)
// strcmp(mid,char)<0              strcmp(mid,char)==0               strcmp(mid,char)>0

int search_by_title(char ti[200])
{
    int first = 0, last = book_count-1, mid, result = -1;

    while(first<=last)
    {
        mid = (first + last)/2;
        int value = strcasecmp(b[mid]->title,ti);


        if(value == 0)
        {
            // Found! Get the original index in the main array
            result = b[mid] - book; // book stores the memory address of the 1st index, as it work as a pointer to the 1st element
            break;
        }
        /*sizeof(struct book_info) = 160 bytes (for simplicity)

            The array starts at 0x1000
            Then the books are stored at these addresses:
            Index	Address Calculation	Memory Address
            book[0]	->0x1000 + (0 * 160) =	0x1000
            book[1]	->0x1000 + (1 * 160) =	0x10A0
            book[2]	->0x1000 + (2 * 160) =	0x1140
            book[3]	->0x1000 + (3 * 160) =	0x11E0
            now,
            &book[3]-book
            = (0x11E0 - 0x1000) / sizeof(struct book_info)
            = (480 bytes) / (160 bytes) = 3 -> it will return the original index.*/

        else if(value > 0)
        {
            last = mid-1;
        }

        else
        {
            first = mid+1;
        }
    }
    return result;
}

void sorted_by_author()
{
    b = malloc(book_count * sizeof(struct book_info *));
    if (b==NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    for (int i = 0; i < book_count; i++) {
        b[i] = &book[i];
    }

    int i, j;
    for (i = 1; i < book_count; i++)
    {
        struct book_info *key = b[i];
        j = i - 1;

        while (j >= 0 && strcasecmp(b[j]->author, key->author) > 0)
        {
            b[j + 1] = b[j];
            j--;
        }
        b[j + 1] = key;
    }
}

int search_by_author(char au[200])
{
    sorted_by_author();

    // Find first occurrence -> the 1st position by the author name
    int first = 0, last = book_count - 1;
    int result = -1;

    while (first <= last)
    {
        int mid = (first + last) / 2;
        int value = strcasecmp(b[mid]->author, au);

        if (value == 0)
        {
            result = mid;
            last = mid - 1; // Look for earlier occurrences
        }
        else if (value > 0)
        {
            last = mid - 1;
        }
        else
        {
            first = mid + 1;
        }
    }

    return result;
}

int is_id_unique(int id)
{
    // Ensure books are sorted by ID first
    sorted_by_ID();

    // Binary search to check if ID exists
    int pos = search_by_ID(id);
    return pos;  // -1 means unique
}

int is_title_unique(char ti[200])
{
    sorted_by_title();

    int pos = search_by_title(ti);
    return pos;
}

int is_author_unique(char au[200])
{
    sorted_by_author();

    int pos = search_by_author(au);
    return pos;
}

void add_book()
{
    int n;
    n = verify_pos_integer("Enter total books to add(or 0 to cancel): ");
    if(n==0) return;

    struct book_info *temp;
    temp=(struct book_info*)realloc(book, (book_count+n) * sizeof(struct book_info));
    //no need to use malloc, since realloc Does the Job of malloc When Needed.

    if(temp==NULL)
    {
        printf("Memory error. Cannot add books.\n");
        exit(1);
    }

    book = temp;
    /*If realloc fails, 'book' becomes NULL, and the old memory is leaked!
    so Using a temporary pointer (temp) to hold the result of realloc.
    original pointer will Update only after success.*/


    for(int i = book_count; i < book_count+n; i++)
    {
        printf("\nEnter details for book [%d]--\n", i+1);
        printf("(Leave title/author empty to cancel the book)\n\n");

        while(1)
        {
            book[i].book_id = verify_pos_integer("Book ID: ");


            if(is_id_unique(book[i].book_id)!= -1)
                printf("ID %d is already in use. Please try a different ID.\n", book[i].book_id);

            else
                break;
        }

        int c;
        while((c = getchar()) != '\n' && c != EOF);//c = getchar() and c!=\n. here c is the return value of getchar

        /*getchar returns a integer value. mostly the ascii value of the characters.

         '\n'(newline) = 10

         ' ' (space) = 32

         EOF(End of the file,No input--->ctrl+D/ctrl+Z) = -1
         EOF means No more data is available to read from a file.
         If a user triggers an EOF signal the prgram will end without a newline characeter.
         so the EOF should also be checked */

        while(1)
        {
            printf("Book title: ");
            fgets(book[i].title,sizeof(book[i].title),stdin);
            book[i].title[strcspn(book[i].title, "\n")] = '\0';// remove the newline character

            printf("Book's author: ");
            fgets(book[i].author,sizeof(book[i].author),stdin);
            book[i].author[strcspn(book[i].author, "\n")] = '\0';

            if(is_author_unique(book[i].author)!= -1 && is_title_unique(book[i].title) != -1)
                printf("This book is already exist. Please add another book\n");

            else
                break;
        }

        if(book[i].title[0] =='\0' || book[i].title[0] ==' ' || book[i].author[0] =='\0' || book[i].author[0] ==' ')// only the 1st character is checked here. so no need to use strcasecmp.
        {
            printf("\nBook entry canceled.\n");
            printf("Options:\n");
            printf("1. Try entering this book again\n");
            printf("2. Continue with next book \n");
            printf("3. Cancel entry & return to main menu\n");

            int choice = verify_pos_integer("Your choice (1-3): ");;

            switch(choice)
            {
                case 1:
                    i--; // Stay on same book index
                    continue;

                case 2:
                    continue; // Move to next book

                case 3:
                    return; // Exit back to main menu

                default:
                    printf("Invalid choice! Your choice must be between 1-3.");
            }
        }
    }
    book_count=book_count+n;
    printf("Books added successfully.\n");
}


void remove_book()
{
    int value, pos;
    while(1)
    {
        value = verify_pos_integer("Enter the book ID to remove(or 0 to cancel): ");
        if (value == 0) return;

        pos = search_by_ID(value);
        if(pos != -1)
            break;

        printf("Error: No book found with ID %d.\n", value);
        printf("Available IDs: ");
        for (int i = 0; i < book_count; i++)
        {
            printf("%d\n", book[i].book_id);
        }
        printf("Please try again or enter 0 to cancel.\n");
    }

    // Show book details before removal
    printf("\nBook to be removed:\n");
    printf("ID: %d, Title: %s, Author: %s\n", book[pos].book_id, book[pos].title, book[pos].author);

    // Get confirmation
    char ch[10];
    int a;
    while((a = getchar()) != '\n' && a != EOF);

    while(1)
    {
        printf("\nAre you sure you want to remove this book? (yes/no): ");
        fgets(ch, sizeof(ch), stdin);
        ch[strcspn(ch, "\n")] = '\0';

        if(strcasecmp(ch,"Yes") != 0 || strcasecmp(ch,"No") != 0)
            printf("You need to answer it with yes or no");

        else break;
    }

    if(strcasecmp(ch,"No") == 0)
    {
        printf("No book is removed.\n");
        return;
    }

    for(int i = pos; i<book_count-1; i++ )
    {
        book[i].book_id = book[i+1].book_id;
        strcpy(book[i].title, book[i+1].title);
        strcpy(book[i].author, book[i+1].author);
    }
    book_count=book_count-1;

    struct book_info *temp;
    temp=(struct book_info*)realloc(book, book_count * sizeof(struct book_info));
    if(temp==NULL)
    {
        printf("Warning: Book removed but memory not resized\n");
        exit(1);
    }

    book = temp;
    printf("Book removed successfully.\n");
}

void display_book()
{
    printf("Available books in the library\n");
    printf("%-5s %-10s %-30s %-20s\n", "No.", "ID", "Title", "Author");
    printf("----------------------------------------------------\n");

    for(int i=0; i<book_count; i++)
    {
        printf("%-5d %-10d %-30s %-20s\n", i+1, book[i].book_id, book[i].title, book[i].author);
    }
}

void Clear_all()
{
    free(book);
    book = NULL;
    book_count = 0;
}

int main()
{
    load_filename_from_config();
    load_books_from_file();

    while(1)
    {
        printf("\n  LIBRARY MANAGEMENT SYSTEM\n");
        printf("------------------------------\n");
        printf("1. Add new books\n");
        printf("2. Remove books\n");
        printf("3. Display books\n");
        printf("4. Search for a book by title\n");
        printf("5. Search for books by author name\n");
        printf("6. Search for a book by ID\n");
        printf("7. Clear all books\n");
        printf("8. Exit\n");


        char ti[200], ch[10];;
        int op = verify_pos_integer("Please choose an option (1-8): ");
        int c, pos;

        switch(op)
        {
            case 1:
                printf("\n");
                add_book();
                break;

            case 2:
                printf("\n");
                if(is_empty())
                    {
                        printf("Book list is empty. Nothing to remove.\n");
                        break;
                    }
                sorted_by_ID();
                remove_book();
                break;

            case 3:
                printf("\n");
                if(is_empty())
                {
                    printf("No books are available. The list is currently empty.\n");
                    break;
                }
                sorted_by_ID();
                display_book();
                break;

            case 4:
                printf("\n");
                if(is_empty())
                    {
                        printf("No books are available at this moment.\n");
                    break;
                    }

                while((c = getchar()) != '\n' && c != EOF);

                printf("Enter the book title that you want to find: ");
                fgets(ti, sizeof(ti), stdin);
                ti[strcspn(ti, "\n")] = '\0';

                sorted_by_title();
                pos = search_by_title(ti);

                if(pos==-1)
                    printf("No book found with the title.\n");

                else
                    printf("Book found at %d no. position, ID no- %d\n",pos+1,book[pos].book_id);

                free(b);  // Free the pointer array here
                b = NULL;
                break;

            case 5:
                printf("\n");
                if(is_empty())
                {
                    printf("Book list is empty. Add some books then search.\n");
                    break;
                }

                while((c = getchar()) != '\n' && c != EOF);

                printf("Enter the author name to search: ");
                fgets(ti, sizeof(ti), stdin);
                ti[strcspn(ti, "\n")] = '\0';

                sorted_by_author();
                int pos = search_by_author(ti);

                if (pos == -1)
                {
                    printf("No books found by author: %s\n", ti);
                    break;
                }

                // Display all books by this author
                printf("\nBooks by %s:\n", ti);
                printf("%-10s %-30s\n", "ID", "Title");
                printf("----------------------------------------\n");

                int count = 0;
                for (int i = pos; i < book_count; i++) {
                    if (strcasecmp(b[i]->author, ti) == 0) {
                        printf("%-10d %-30s\n", b[i]->book_id, b[i]->title);
                        count++;
                    } else {
                        break; // Authors are sorted, so we can break when we find a different one
                    }
                }

                printf("Total books found: %d\n", count);


                free(b); // free the pointer array
                b = NULL;
                break;

            case 6:
                printf("\n");
                if(is_empty())
                {
                    printf("No books are available at this moment.\n");
                    break;
                }

                int search_id = verify_pos_integer("Enter the book ID to search: ");
                sorted_by_ID();
                pos = search_by_ID(search_id);

                if(pos == -1)
                    printf("No book found with ID %d.\n", search_id);

                else
                {
                    printf("Book found:\n");
                    printf("ID: %d, Title: %s, Author: %s\n",
                        book[pos].book_id, book[pos].title, book[pos].author);
                }
                break;

            case 7:
                printf("\n");
                if(is_empty())
                    {
                        printf("Book list is already empty.\n");
                        break;
                    }

                // get confirmation
                while((c = getchar()) != '\n' && c != EOF);

                while(1)
                {
                    printf("Are you sure you want to reset everything? (yes/no): ");
                    fgets(ch, sizeof(ch), stdin);
                    ch[strcspn(ch, "\n")] = '\0';

                    if(strcasecmp(ch,"Yes") != 0 || strcasecmp(ch,"No") != 0)
                        printf("You need to answer it with yes or no");

                    else break;
                }

                if(strcasecmp(ch,"Yes") == 0)
                {
                    Clear_all();
                    printf("Book list have been cleared.\n");
                }
                else
                    printf("everything remain as it is.");

                break;

            case 8:
                printf("\n");

                //get confirmation
                while((c = getchar()) != '\n' && c != EOF);
                while(1) {
                    printf("Do you want to save before exiting? (yes/no/cancel): ");
                    fgets(ch, sizeof(ch), stdin);
                    ch[strcspn(ch, "\n")] = '\0';

                    if (strcasecmp(ch, "yes") == 0 || strcasecmp(ch, "y") == 0)
                    {
                        save_books_to_file();
                        printf("Thank you for using the library management system.\n");
                        printf("Exiting\n");
                        exit(0);
                    }
                    else if (strcasecmp(ch, "no") == 0 || strcasecmp(ch, "n") == 0)
                    {
                        printf("Thank you for using the library management system.\n");
                        printf("Exiting\n");
                        exit(0);
                    }
                    else if (strcasecmp(ch, "cancel") == 0 || strcasecmp(ch, "c") == 0)
                    {
                        printf("Exit canceled. Returning to main menu.\n");
                        break;
                    }
                    else {
                        printf("Please answer 'yes', 'no', or 'cancel'.\n");
                    }
                }
                break;


            default:
                printf("Your choice must be between 1 to 7. Please try again.\n");
        }
    }

return 0;
}
