/* 编写一个程序，根据与一特定用户或组相对应的 ACE 来显示权限。该程序应接受
2 个命令行参数。第一个参数可以为字母“u”或“g”，用以表明第二个参数是用
户还是组。（利用定义于程序清单 8-1 中的函数，还可将第二个参数任意指定为数
字或名称。）若与给定用户或组相对应的 ACE 隶属于组分类，则程序还需另外显示
与 ACL 掩码型记录相与后的权限。 */

/*
我只实现了对用户的权限判断的一部分,剩下的的部分代码很长但没什么本质上的区别.
参考17.2节的ACL权限检查算法.
可以通过stat()获取文件的属主和属组.
 */

#include <acl/libacl.h>
#include <sys/acl.h>
#include <sys/stat.h>

#include "tlpi_hdr.h"

acl_entry_t *get_acl_mask(acl_t *);

int main(int argc, char *argv[]) {
    const char *usage = "%s u/g id [-d] file\n";
    if (argc != 4 && argc != 5) usageErr(usage, argv[0]);
    if (argv[1][0] != 'g' && argv[1][0] != 'u') usageErr(usage, argv[0]);
    char *path = argv[3];
    if (!strcmp(argv[3], "-d")) path = argv[4];

    // get ACL from file
    acl_type_t type = ACL_TYPE_ACCESS;
    if (argc == 5) type = ACL_TYPE_DEFAULT;  // -d
    acl_t acl = acl_get_file(path, type);
    if (acl == NULL) errExit("acl_get_file");

    // show ACL
    char buf[BUFSIZ];
    long l = BUFSIZ;
    puts(acl_to_text(acl, &l));

    char rwx[4] = {'-', '-', '-', '\0'};
    acl_entry_t entry;
    if (argv[1][0] == 'u') {
        uid_t uid = atoi(argv[2]);
        // 1. if user is root, all permission except x for some situation
        // (see 17.2)
        if (uid == 0) {  // root user
            rwx[0] = 'r';
            rwx[1] = 'w';
            for (int entid = ACL_FIRST_ENTRY; rwx[2] == '-';
                 entid = ACL_NEXT_ENTRY) {  // retrive ACEs
                int ret = acl_get_entry(acl, entid, &entry);
                if (ret == -1) errExit("acl_get_entry");
                if (ret == 0) break;

                acl_permset_t perm;
                if (acl_get_permset(entry, &perm) == -1)
                    errExit("acl_get_permset");
                ret = acl_get_perm(perm, ACL_EXECUTE);
                if (ret == -1) errExit("acl_get_perm");
                rwx[2] = ret ? 'x' : '-';
            }
        } else {             // non-root user
            struct stat fi;  // file info
            if (stat(path, &fi) == -1) errExit("stat");
            uid_t owner_uid = fi.st_uid;
            // gid_t owner_gid = fi.st_gid;

            // 2. if uid matches owner_uid, gets permset from ACL_USER_OBJ
            acl_tag_t tag_type;
            if (uid == owner_uid) {
                for (int entid = ACL_FIRST_ENTRY;; entid = ACL_NEXT_ENTRY) {
                    int ret = acl_get_entry(acl, entid, &entry);
                    if (ret == -1) errExit("acl_get_entry");
                    if (ret == 0) break;

                    if (acl_get_tag_type(entry, &tag_type) == -1)
                        errExit("acl_get_tag_type");
                    if (tag_type != ACL_USER_OBJ) continue;

                    acl_permset_t perm;
                    if (acl_get_permset(entry, &perm) == -1)
                        errExit("acl_get_permset");

                    ret = acl_get_perm(perm, ACL_READ);
                    if (ret == -1) errExit("acl_get_perm");
                    rwx[0] = ret ? 'r' : '-';
                    ret = acl_get_perm(perm, ACL_WRITE);
                    if (ret == -1) errExit("acl_get_perm");
                    rwx[1] = ret ? 'w' : '-';
                    ret = acl_get_perm(perm, ACL_EXECUTE);
                    if (ret == -1) errExit("acl_get_perm");
                    rwx[2] = ret ? 'x' : '-';

                    break;
                }
            } else {
                // 3. if uid matches some ACL_USER, give its permset & ACL_MASK
                // permset
                for (int entid = ACL_FIRST_ENTRY;; entid = ACL_NEXT_ENTRY) {
                    int ret = acl_get_entry(acl, entid, &entry);
                    if (ret == -1) errExit("acl_get_entry");
                    if (ret == 0) break;

                    if (acl_get_tag_type(entry, &tag_type) == -1)
                        errExit("acl_get_tag_type");
                    if (tag_type != ACL_USER) continue;

                    uid_t *qualp = (uid_t*)acl_get_qualifier(entry);
                    if (qualp == NULL) errExit("acl_get_qualifier");
                    if (*qualp != uid) continue;

                    acl_entry_t *mask = get_acl_mask(&acl);
                    if (mask == NULL) errExit("mask not found");

                    acl_permset_t user_perm, mask_perm;
                    if (acl_get_permset(entry, &user_perm) == -1)
                        errExit("acl_get_permset");
                    if (acl_get_permset(*mask, &mask_perm) == -1)
                        errExit("acl_get_permset");
                    
                    errno = 0;
                    if (acl_get_perm(user_perm, ACL_READ) == 1 && acl_get_perm(mask_perm, ACL_READ) == 1)
                        rwx[0] = 'r';
                    if (acl_get_perm(user_perm, ACL_WRITE) == 1 && acl_get_perm(mask_perm, ACL_WRITE) == 1)
                        rwx[1] = 'w';
                    if (acl_get_perm(user_perm, ACL_EXECUTE) == 1 && acl_get_perm(mask_perm, ACL_EXECUTE) == 1)
                        rwx[2] = 'x';
                    if (errno != 0) errExit("some error");
                    
                    break;
                }
            }
        }

        printf("user %d has %s for '%s'\n", uid, rwx, path);
    } else {  // group
        gid_t gid = atoi(argv[2]);

        // ... a lot of work 
    }

    return 0;
}

acl_entry_t *get_acl_mask(acl_t *acl) {
    static acl_entry_t mask;
    static int done = 0;

    if (done) return &mask;

    for (int entid = ACL_FIRST_ENTRY;; entid = ACL_NEXT_ENTRY) {
        int ret = acl_get_entry(*acl, entid, &mask);
        if (ret == -1) errExit("acl_get_entry");
        if (ret == 0) return NULL;  // no mask entry found

        acl_tag_t tag_type;
        if (acl_get_tag_type(mask, &tag_type) == -1)
            errExit("acl_get_tag_type");
        if (tag_type == ACL_MASK) {
            done = 1;
            break;
        }
    }

    return &mask;
}