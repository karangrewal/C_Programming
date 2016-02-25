#include "friends.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/************************* HELPER FUNCTION PROTOTYPES ***********************/


void print_post(Post *post);

void print_profile_pic(User *user);

void print_friends(User *user);

void print_line();

int are_friends(User *u1, User *u2);

void remove_friend(User *user, User *friend);

void remove_posts(User *user);


/***************************** FRIENDS FUNCTIONS ****************************/


/*
 * Create a new user with the given name.  Insert it at the tail of the list
 * of users whose head is pointed to by *user_ptr_add.
 *
 * Return:
 *   - 0 if successful
 *   - 1 if a user by this name already exists in this list
 *   - 2 if the given name cannot fit in the 'name' array
 *       (don't forget about the null terminator)
 */
int create_user(const char *name, User **user_ptr_add) {

    if (strlen(name) > MAX_NAME) {
	return 2;
    } else if (find_user(name, *user_ptr_add) != NULL) {
	return 1;
    }

    // allocate space for a new user on the heap
    User *new_user = malloc(sizeof(User));

    strncpy(new_user->name, name, MAX_NAME);
    strncpy(new_user->profile_pic, "NULL", MAX_NAME);
    new_user->first_post = NULL; // set intial value to NULL
    new_user->friends[0] = 0; // set value to 0 to indicate end of user's
				// friends in friends array
    new_user->next = NULL; // set initial value to NULL

    // add new_user to user_ptr_add
    User *curr = *user_ptr_add;
    User *prev = NULL;

    // traverse *user_ptr_add until end is reached
    while (curr) {
	prev = curr;
	curr = curr->next;
    }

    if (prev) {
	prev->next = new_user;
    } else {
	new_user->next = *user_ptr_add;
	*user_ptr_add = new_user;
    }

    return 0;
}


/*
 * Return a pointer to the user with this name in
 * the list starting with head. Return NULL if no such user exists.
 *
 * NOTE: You'll likely need to cast a (const User *) to a (User *)
 * to satisfy the prototype without warnings.
 */
User *find_user(const char *name, const User *head) {

    User *pt = (User *)head;

    while (pt != NULL && strcmp(pt->name, name) != 0) {
	pt = pt->next;
    }
    
    return pt;
}


/*
 * Print the usernames of all users in the list starting at curr.
 * Names should be printed to standard output, one per line.
 */
void list_users(const User *curr) {
    
    fprintf(stdout, "User List\n");

    // traverse curr, printing each user
    while (curr) {
	fprintf(stdout, "        %s\n", curr->name);
	curr = curr->next;
    }
}


/*
 * Change the filename for the profile pic of the given user.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if the file does not exist.
 *   - 2 if the filename is too long.
 */
int update_pic(User *user, const char *filename) {

    if (strlen(filename) > MAX_NAME) {
	return 2;
    }
    
    FILE *new_pic = fopen(filename, "r");

    // check for errors while opening the file pointed to by filename
    if (new_pic == NULL) {
	return 1;
    }

    strncpy(user->profile_pic, filename, MAX_NAME);
    fclose(new_pic);

    return 0;
}


/*
 * Make two users friends with each other.  This is symmetric - a pointer to
 * each user must be stored in the 'friends' array of the other.
 *
 * New friends must be added in the first empty spot in the 'friends' array.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if the two users are already friends.
 *   - 2 if the users are not already friends, but at least one already has
 *     MAX_FRIENDS friends.
 *   - 3 if the same user is passed in twice.
 *   - 4 if at least one user does not exist.
 *
 * Do not modify either user if the result is a failure.
 * NOTE: If multiple errors apply, return the *largest* error code that applies.
 */
int make_friends(const char *name1, const char *name2, User *head) {

    // check to see whether the name user has been passed twice
    if (strcmp(name1, name2) == 0) {
	return 3;
    }
    
    User *curr = head;
    User *friend1 = NULL;
    User *friend2 = NULL;
    
    // traverse head to find the appropriate users
    while (curr) {

	if (strcmp(curr->name, name1) == 0) {
	    friend1 = curr;
	} else if (strcmp(curr->name, name2) == 0) {
	    friend2 = curr;
	}
	curr = curr->next;

    }

    // check if either friend1 or friend2 are NULL pointers
    if (!friend1 || !friend2) {
	return 4;
    }

    // check if *friend1 and *friend2 are already friends
    if (are_friends(friend1, friend2) == 0) {
	return 1;
    }

    // friend counters
    int num_friends1 = 0;
    int num_friends2 = 0;

    // check whether friend2 has MAX_FRIENDS friends
    while (num_friends2 < MAX_FRIENDS) {
	if ((friend2->friends)[num_friends2] == 0) {
	    break;
	}
	num_friends2++;
    }

    // check whether friend1 has MAX_FRIENDS friends
    while (num_friends1 < MAX_FRIENDS) {
	if ((friend1->friends)[num_friends1] == 0) {
	    break;
	}
	num_friends1++;
    }

    if (num_friends1 == MAX_FRIENDS || num_friends2 == MAX_FRIENDS) {
	return 2;
    }

    /*
    Make friend1 and friend2 friends. Once a friend has been added to the
    other friend's array of friends / users, set their friend counter to -1 to
    indicate the task has been completed.
    */

    num_friends1 = 0;
    num_friends2 = 0;
    while (num_friends1 >= 0 || num_friends2 >= 0) {

	if (num_friends1 >= 0) {

	    if ((friend1->friends)[num_friends1] == 0) {
		(friend1->friends)[num_friends1] = friend2;

		if (num_friends1 < MAX_FRIENDS - 1) {
		    (friend1->friends)[num_friends1 + 1] = 0;
	        }

		// completed task for friend1
		num_friends1 = -1;

	    } else {
		num_friends1++;
	    }

	}
	
	if (num_friends2 >= 0) {

	    if ((friend2->friends)[num_friends2] == 0) {
		(friend2->friends)[num_friends2] = friend1;

		if (num_friends2 < MAX_FRIENDS - 1) {
		    (friend2->friends)[num_friends2 + 1] = 0;
	        }

		// completed task for friend2
		num_friends2 = -1;

	    } else {
		num_friends2++;

	    }

	}
    }

    return 0;
}


/*
 * Print a user profile.
 * For an example of the required output format, see the example output
 * linked from the handout.
 * Return:
 *   - 0 on success.
 *   - 1 if the user is NULL.
 */
int print_user(const User *user) {

    if (user) {
	
	/*
	Call helper functions to sequentially print out the profile of the
	user pointed to by user.
	*/

	// user's profile pic and name
	print_profile_pic((User*) user);
	printf("Name: %s\n\n", user->name);
	print_line();

	// user's friends
	printf("Friends:\n");
	print_friends((User *) user);
	print_line();

	// user's posts
	printf("Posts:\n");
	Post *cur_post = user->first_post;

	while (cur_post) {

	    print_post(cur_post);

	    if (cur_post->next) {
		printf("\n===\n\n");
	    }

	    cur_post = cur_post->next;

	}

	print_line();
	return 0;
    }

    return 1;
}


/*
 * Make a new post from 'author' to the 'target' user,
 * containing the given contents, IF the users are friends.
 *
 * Insert the new post at the *front* of the user's list of posts.
 *
 * Use the 'time' function to store the current time.
 *
 * 'contents' is a pointer to heap-allocated memory - you do not need
 * to allocate more memory to store the contents of the post.
 *
 * Return:
 *   - 0 on success
 *   - 1 if users exist but are not friends
 *   - 2 if either User pointer is NULL
 */
int make_post(const User *author, User *target, char *contents) {
    
    if (author && target) {

	if (are_friends((User *) author, target) == 0) {

	    Post *new_second_post = target->first_post;

	    // allocate space for a new post on the heap
	    Post *new_post = malloc(sizeof(Post));
	    strncpy(new_post->author, author->name, MAX_NAME);
	    new_post->contents = contents;

	    // allocate space on the heap for time
	    time_t *cur_time = malloc(sizeof(time_t));
	    *cur_time = time(NULL);
	    new_post->date = cur_time;

	    new_post->next = new_second_post;
	    target->first_post = new_post;

	    return 0;

	} else {

	    return 1;

	}
    }

    return 2;
}


/*
 * From the list pointed to by *user_ptr_del, delete the user
 * with the given name.
 * Remove the deleted user from any lists of friends.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if a user with this name does not exist.
 */
int delete_user(const char *name, User **user_ptr_del) {
    
    User *curr = *user_ptr_del;
    User *prev = NULL;

    while (curr) {

	if (strcmp(curr->name, (char *) name) == 0) {
	    
	    // found user to delete
	    if (prev) {
		prev->next = curr->next;
	    } else {
		*user_ptr_del = curr->next;
	    }

	    int k = 0;
	    while (k < MAX_FRIENDS) {
		if ((curr->friends)[k] == 0) {
		    break;
		} else {
		    remove_friend((curr->friends)[k], curr);
		}
		k++;
	    }

	    // free memory allocated to this user, including posts
	    remove_posts(curr);
	    free(curr);
	    return 0;
	}
	
	prev = curr;
	curr = curr->next;
    }

    return 1;

}


/****************************** HELPER FUNCTIONS *****************************/


/*
 * Print the post pointed to by post.
 *
 */
void print_post(Post *post) {

    printf("From: %s\n", post->author);
    printf("Date: %s\n", ctime(post->date));
    printf("%s\n", post->contents);

}


/*
 * Print the profile pic of the user pointed to by user, if the user has one.
 *
 */
void print_profile_pic(User *user) {

    if (strcmp(user->profile_pic, "NULL") != 0) {

	FILE *f = fopen(user->profile_pic, "r");
	char line[127];

	while (fgets(line, 127, f)) {
	    printf("%s", line);
	}

	printf("\n\n");
	fclose(f);
    }

}


/*
 * Print all friends of user.
 *
 */
void print_friends(User *user) {
    
    int i = 0;

    while (user->friends[i] != 0) {
	printf("%s\n", (user->friends[i])->name);
	i++;
    }

}


/*
 * Print 42 occurances of '-' on one single line.
 *
 */
void print_line() {
    
    for (int i = 0; i < 42; i++) {
	printf("-");
    }

    printf("\n");

}


/*
 * Return 0 if u1 and u2 are friends, 1 otherwise.
 *
 */
int are_friends(User *u1, User *u2) {

    int i = 0;
    User *cur_friend;
    while (i < MAX_FRIENDS) {
	
	cur_friend = (u1->friends)[i];

	if (cur_friend == 0) {
	    return 1; // false
	} else if (strcmp(cur_friend->name, u2->name) == 0) {
	    return 0; // true
	}
	i++;
    }

    return 1; // false

}


/*
 * Delete friend from user's array of friends.
 *
 */
void remove_friend(User *user, User *friend) {

    int i = 0;
    User *cur_friend;

    while (i < MAX_FRIENDS) {
	
	cur_friend = (user->friends)[i];
	if (cur_friend == 0) {
	    break;
	} else if (strcmp(cur_friend->name, friend->name) == 0) {
	    
	    // found friend to remove
	    int j = i;
	    int flag = 0;
	    while (j + 1 < MAX_FRIENDS && flag == 0) {
		
		if ((user->friends)[j] != 0) {
		    (user->friends)[j] = (user->friends)[j + 1];
		} else {
		    flag = 1;
		}
		j++;
	    }
	}
	i++;
    }
}


/*
 * Delete all posts by user and free up all heap space allocated to the post
 * and any of its contents.
 *
 */
void remove_posts(User *user) {

    Post *cur_post = user->first_post;
    Post *prev_post = NULL;

    while (cur_post || prev_post) {
	if (prev_post) {
	    free(prev_post->contents);
	    free(prev_post->date);
	    free(prev_post);
	}

	prev_post = cur_post;
	if (cur_post) {
	    cur_post = cur_post->next;
	}
    }
}

