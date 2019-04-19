// Modify list of active groups in the current shell
// By Gregory Touretsky heavily leveraging https://github.com/joshuatshaffer/fusuma-suid-wrapper code
// Usage: setgroup +newgroup -oldgroup cmd args
// Example: setgroup +users2 /bin/bash

#include <errno.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

gid_t get_gid (char *name) {
    errno = 0;
    struct group *group_info = getgrnam (name);
    if (group_info == NULL) {
        if (errno == 0) {
            fprintf (stderr, "The group \"%s\" could not be found.\n", name);
        } else {
            perror ("Could not get group info");
        }
    }
    return group_info->gr_gid;
}

void join_group (gid_t group_to_add) {
    // Get the groups we are currently in.
    int num_groups = getgroups (0, NULL);
    gid_t *groups = calloc (num_groups + 1, sizeof (gid_t));
    getgroups (num_groups, groups + 1);

    // Add the new group.
    groups[0] = group_to_add;

    setgroups (num_groups + 1, groups);
    fprintf(stdout, "Added group %d.\n", group_to_add);
}

void drop_group (gid_t group_to_drop) {
    // Get the groups we are currently in.
    int num_groups = getgroups (0, NULL);
    gid_t *groups = calloc (num_groups, sizeof (gid_t));
    gid_t *newgroups = calloc (num_groups, sizeof (gid_t));
    getgroups (num_groups, groups);

    int gidFound = 0, k=0;
    // Remove the group
    for(int i=0;i < num_groups; i++) {
        if(groups[i] == group_to_drop) {
            gidFound = 1;
            fprintf(stdout, "Removed group %d.\n", group_to_drop);

        } else {
            // keep the GID in the list
            newgroups[k]=groups[i];
            k++;
        }
    }

    if(!gidFound) {
        fprintf (stderr, "The GID %d could not be found.\n", group_to_drop);
    }

    setgroups (k + 1, newgroups);
}

void drop_sudo () {
    uid_t uid = getuid ();
    seteuid (uid);
}

void exec_pass_args (char *file, int argc, char **argv) {
    // Make a copy of argv including the NULL terminator.
    char **argv_new = calloc (argc + 1, sizeof (char **));
    memcpy (argv_new, argv, (argc + 1) * sizeof (char **));

    // Set the program name arg.
    argv_new[0] = file;

    execvp (file, argv_new);
    perror ("execvp returned");
}

int main (int argc, char *argv[]) {

    // basic validation of parameters
    if(argc < 2) {
        fprintf(stderr, "Not enough arguments. Usage:\n");
        fprintf(stderr, "   setgroups {+|-}group1 [{+|-}group2...] <command> [attributes]\n");
        exit(1);
    }
    // process all group changes
    int in_group_list = 1;
    int i=0;

    while(in_group_list) {
        i++;
        if(argv[i][0] == '+') {
            //add new group
            gid_t new_gid = get_gid (&argv[i][1]);
            join_group (new_gid);
        } else {
            if(argv[i][0] == '-') {
                //remove new group
                gid_t new_gid = get_gid (&argv[i][1]);
                drop_group (new_gid);
            } else {
                //no more groups, exiting
                in_group_list = 0;
            }
        }
    }
    
    // Drop sudo privaleges.
    // They are no longer needed and should not be passed on to the next stage.
    drop_sudo ();

    // Exec the command and pass along the command line args.
    exec_pass_args (argv[i], argc-i, &argv[i]);
}
