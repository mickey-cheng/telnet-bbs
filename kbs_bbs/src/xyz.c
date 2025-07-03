/*
 This file has been checked global varible.
 һЩ����Ĺ���
*/

#include "bbs.h"

#ifdef SECONDSITE
void myexec_cmd(int umode,const char *cmdfile, const char *param);
#endif
extern int convcode;            /* KCN,99.09.05 */

int modify_user_mode(int mode)
{
    int ret=uinfo.mode;
    if (uinfo.mode!=mode) {
        uinfo.mode=mode;
        UPDATE_UTMP(mode,uinfo);
    }
    return ret;
}

/*
int
x_csh()
{
    int save_pager;
    clear() ;
    refresh() ;
    reset_tty() ;
    save_pager = uinfo.pager;
    uinfo.pager = 0 ;
    UPDATE_UTMP(pager,uinfo);
    bbslog("user","%s","shell out");
#ifdef SYSV
    do_exec("sh", NULL) ;
#else
    do_exec("csh", NULL);
#endif
    restore_tty() ;
    uinfo.pager = save_pager;
    UPDATE_UTMP(pager,uinfo);
    clear() ;
    return 0 ;
}

*/

struct _setperm_select {
    unsigned int pbits;
    unsigned int basic;
    unsigned int oldbits;
};
int showperminfo(struct _select_def *conf, int i)
{
    struct _setperm_select *arg = (struct _setperm_select *) conf->arg;

    i = i - 1;
    if (i == conf->item_count - 1) {
        prints("%c. �˳� ", 'A' + i);
    } else if ((arg->pbits & (1 << i)) != (arg->oldbits & (1 << i))) {
        prints("%c. %-27s \033[31;1m%3s\033[m", 'A' + i, permstrings[i], ((arg->pbits >> i) & 1 ? "ON" : "OFF"));
    } else {
        prints("%c. %-27s \x1b[37;0m%3s\x1b[m", 'A' + i, permstrings[i], ((arg->pbits >> i) & 1 ? "ON" : "OFF"));
    }
    return SHOW_CONTINUE;
}

#ifdef SMS_SUPPORT
int showsmsdef(struct _select_def *conf, int i)
{
    struct _setperm_select *arg = (struct _setperm_select *) conf->arg;

    i = i - 1;
    if (i == conf->item_count - 1) {
        prints("%c. �˳� ", 'A' + i);
    } else if ((arg->pbits & (1 << i)) != (arg->oldbits & (1 << i))) {
        prints("%c. %-40s \033[31;1m%3s\033[m", 'A' + i, user_smsdefstr[i], ((arg->pbits >> i) & 1 ? "ON" : "OFF"));
    } else {
        prints("%c. %-40s \x1b[37;0m%3s\x1b[m", 'A' + i, user_smsdefstr[i], ((arg->pbits >> i) & 1 ? "ON" : "OFF"));
    }
    return SHOW_CONTINUE;
}
#endif

int showuserdefine1(struct _select_def *conf, int i)
{
    struct _setperm_select *arg = (struct _setperm_select *) conf->arg;

    i = i - 1;
    if (i == conf->item_count - 1) {
        prints("%c. �˳� ", 'A' + i);
    } else if ((arg->pbits & (1 << i)) != (arg->oldbits & (1 << i))) {
        prints("%c. %-27s \033[31;1m%3s\033[m", 'A' + i, user_definestr[i+32], ((arg->pbits >> i) & 1 ? "ON" : "OFF"));
    } else {
        prints("%c. %-27s \x1b[37;0m%3s\x1b[m", 'A' + i, user_definestr[i+32], ((arg->pbits >> i) & 1 ? "ON" : "OFF"));
    }
    return SHOW_CONTINUE;
}

int showuserdefine(struct _select_def *conf, int i)
{
    struct _setperm_select *arg = (struct _setperm_select *) conf->arg;

    i = i - 1;
    if (i == conf->item_count - 1) {
        prints("%c. �˳� ", 'A' + i);
    } else if ((arg->pbits & (1 << i)) != (arg->oldbits & (1 << i))) {
        prints("%c. %-27s \033[31;1m%3s\033[m", 'A' + i, user_definestr[i], ((arg->pbits >> i) & 1 ? "ON" : "OFF"));
    } else {
        prints("%c. %-27s \x1b[37;0m%3s\x1b[m", 'A' + i, user_definestr[i], ((arg->pbits >> i) & 1 ? "ON" : "OFF"));
    }
    return SHOW_CONTINUE;
}

int setperm_select(struct _select_def *conf)
{
    struct _setperm_select *arg = (struct _setperm_select *) conf->arg;

    if (conf->pos == conf->item_count)
        return SHOW_QUIT;
    arg->pbits ^= (1 << (conf->pos - 1));
    return SHOW_REFRESHSELECT;
}

int setperm_show(struct _select_def *conf, int i)
{
    struct _setperm_select *arg = (struct _setperm_select *) conf->arg;

    i = i - 1;
    if (i == conf->item_count - 1) {
        prints("%c. �˳� ", 'A' + i);
    } else {
        if ((arg->pbits & (1 << i)) != (arg->oldbits & (1 << i))) {
            prints("%c. %-27s \033[31;1m%3s\033[m", 'A' + i, permstrings[i], ((arg->pbits >> i) & 1 ? "ON" : "OFF"));
        } else if ((1 << i == PERM_BASIC || 1 << i == PERM_POST || 1 << i == PERM_CHAT || 1 << i == PERM_PAGE || 1 << i == PERM_DENYMAIL|| 1 << i == PERM_DENYRELAX) && (arg->basic & (1 << i)))
            prints("%c. %-27s \033[32;1m%3s\033[m", 'A' + i, permstrings[i], ((arg->pbits >> i) & 1 ? "ON" : "OFF"));
        else
            prints("%c. %-27s \033[37;0m%3s\033[m", 'A' + i, permstrings[i], ((arg->pbits >> i) & 1 ? "ON" : "OFF"));
    }
    return SHOW_CONTINUE;
}

int setperm_key(struct _select_def *conf, int key)
{
    int sel;

    if (key == Ctrl('Q'))
        return SHOW_QUIT;
    if (key == Ctrl('A')) {
        struct _setperm_select *arg = (struct _setperm_select *) conf->arg;

        arg->pbits = arg->oldbits;
        return SHOW_QUIT;
    }
    if (key <= 'z' && key >= 'a')
        sel = key - 'a';
    else
        sel = key - 'A';
    if (sel >= 0 && sel < (conf->item_count)) {
        conf->new_pos = sel + 1;
        return SHOW_SELCHANGE;
    }
    return SHOW_CONTINUE;
}

unsigned int setperms(unsigned int pbits, unsigned int basic, char *prompt, int numbers, int (*show)(struct _select_def *, int), int (*select)(struct _select_def *))
{
    struct _select_def perm_conf;
    struct _setperm_select arg;
    POINT *pts;
    int i;

    pts = (POINT *) malloc(sizeof(POINT) * (numbers + 1));

    move(4, 0);
    prints("�밴����Ҫ�Ĵ������趨%s, Ctrl+Q�˳���Ctrl+A�����޸��˳�.\n", prompt);
    move(6, 0);
    clrtobot();

    for (i = 0; i < numbers + 1; i++) {
        pts[i].x = 0 + ((i > 15) ? 42 : 2);
        pts[i].y = i + 6 - ((i > 15) ? 16 : 0);
    }
    arg.pbits = pbits;
    arg.basic = basic;
    arg.oldbits = pbits;
    memset(&perm_conf, 0, sizeof(struct _select_def));
    perm_conf.item_count = numbers + 1;
    perm_conf.item_per_page = numbers + 1;
    perm_conf.flag = LF_BELL | LF_LOOP; /*|LF_HILIGHTSEL;*/
    perm_conf.prompt = "��";
    perm_conf.item_pos = pts;
    perm_conf.arg = &arg;
    perm_conf.title_pos.x = 1;
    perm_conf.title_pos.y = 6;
    perm_conf.pos = numbers + 1;

    if (select)
        perm_conf.on_select = select;
    else
        perm_conf.on_select = setperm_select;
    perm_conf.show_data = show;
    perm_conf.key_command = setperm_key;

    list_select_loop(&perm_conf);
    free(pts);
    return arg.pbits;
}

/* ɾ�����ڵ��ʺ� */
/* ���Ǹ����˵����� */
/* Bigman 2001.7.14 */
int confirm_delete_id(void)
{
    char buff[STRLEN];
    int usernum;

    modify_user_mode(ADMIN);
    clear();
    move(8, 0);
    prints("\n");
    clrtoeol();
    getdata(9,0,"������Ҫȷ��������û�ID: ", genbuf, IDLEN+1,1,NULL,true);
    if (genbuf[0] == '\0') {
        clear();
        return 1;
    }

    if ((usernum = searchuser(genbuf)) != 0) {
        prints("���ʺ�����ʹ��\n");
        pressreturn();
        return 1;
    }

    sethomepath(buff, genbuf);
    /*
     * sprintf(commd,"rm -rf %s",buff);
     */
    f_rm(buff);
    setmailpath(buff, genbuf);
    f_rm(buff);
    /*
     * sprintf(commd,"rm -rf %s",buff);
     */

    bbslog("user","%s","delete confirmly dead id's directory");

    clrtoeol();
    pressreturn();
    clear();
    return 0;
}

#ifdef SECONDSITE
int x_mj(void)
{
    myexec_cmd(BBSNET,"bin/qkmj",NULL);
    redoscr();
    pressreturn();
    clear();
    return 0;
}
#endif /* SECONDSITE */

int x_level(void)
{
    unsigned int newlevel;
    int flag = 0;               /*Haohmaru,98.10.05 */
    int flag1 = 0, flag2 = 0;   /* bigman 2000.1.5 */
    struct userec *lookupuser;
    int id,basicperm,s[GIVEUPINFO_PERM_COUNT];

    /*
     * add by alex, 97.7 , strict the power of sysop
     */
#ifdef SOURCE_PERM_CHECK
    if (!HAS_PERM(getCurrentUser(), PERM_ADMIN)) {
        move(3, 0);
        clrtobot();
        prints("��Ǹ, ��û�� ADMIN Ȩ��!");
        pressreturn();
        return 0;
    }
#endif
    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return 0;
    }
    clear();
    move(0, 0);
    prints("����" NAME_USER_SHORT "��Ȩ��(\x1b[mע�⣺����Ƿ����⣬��ʹ�÷��ѡ����\x1b[m)\n");
    clrtoeol();
    move(1, 0);
    usercomplete("������" NAME_USER_SHORT " ID: ", genbuf);
    if (genbuf[0] == '\0') {
        clear();
        return 0;
    }
    if (!(id = getuser(genbuf, &lookupuser))) {
        move(3, 0);
        prints("�Ƿ� ID");
        clrtoeol();
        pressreturn();
        clear();
        return 0;
    }
    if ((lookupuser->userlevel & PERM_BOARDS))  /*Haohmaru.98.10.05 */
        flag = 1;
    if ((lookupuser->userlevel & PERM_CLOAK))   /* Bigman 2000.1.5 */
        flag1 = 1;
    if ((lookupuser->userlevel & PERM_XEMPT))
        flag2 = 1;

    /*Bad 2002.7.6 �������������*/
    basicperm=get_giveupinfo(lookupuser,s);
    move(1, 0);
    clrtobot();
    move(2, 0);
    prints("���趨" NAME_USER_SHORT " '%s' ��Ȩ��\n", genbuf);
    newlevel = setperms(lookupuser->userlevel, basicperm, "Ȩ��", NUMPERMS, setperm_show, NULL);
    move(2, 0);
    if (newlevel == lookupuser->userlevel)
        prints(NAME_USER_SHORT " '%s' ��Ȩ��û�и���\n", lookupuser->userid);
    else {                      /* Leeward: 1997.12.02 : Modification starts */
        char secu[STRLEN];

        sprintf(secu, "�޸� %s ��Ȩ��XPERM%d %d", lookupuser->userid, lookupuser->userlevel, newlevel);
        securityreport(secu, lookupuser, NULL, getSession());
        lookupuser->userlevel = newlevel;
        /*
         * Leeward: 1997.12.02 : Modification stops
         */

        prints(NAME_USER_SHORT " '%s' ��Ȩ���Ѹ���,ע�⣺����Ƿ����⣬\n", lookupuser->userid);
        sprintf(genbuf, "changed permissions for %s", lookupuser->userid);
        bbslog("user","%s",genbuf);
        /*
         * Haohmaru.98.10.03.�����ΰ����Զ�����
         */
//        if ((lookupuser->userlevel & PERM_BOARDS) && flag == 0)
//            mail_file(getCurrentUser()->userid, "etc/forbm", lookupuser->userid, "����" NAME_BM "�ض�", BBSPOST_LINK, NULL);
        /*
         * Bigman 2000.1.5 �޸�Ȩ���Զ�����
         */
        if ((lookupuser->userlevel & PERM_CLOAK) && flag1 == 0)
            mail_file(getCurrentUser()->userid, "etc/forcloak", lookupuser->userid, NAME_SYSOP_GROUP "����������Ȩ��", BBSPOST_LINK, NULL);
        if ((lookupuser->userlevel & PERM_XEMPT) && flag2 == 0)
            mail_file(getCurrentUser()->userid, "etc/forlongid", lookupuser->userid, NAME_SYSOP_GROUP "�����������ʺ�Ȩ��", BBSPOST_LINK, NULL);
        save_giveupinfo(lookupuser,s);
    }
    pressreturn();
    clear();
    return 0;
}



/*etnlegend,2005.07.10*/
struct check_level_arg {
    int check_mode;
    int count;
    unsigned int check_level;
    FILE *log_file;
};
static int check_level_func(struct userec *user,struct check_level_arg* arg)
{
    char perm[32];
    if (!user->userid[0])
        return 0;
    if (!arg->check_mode) {
        if ((user->userlevel&arg->check_level)^arg->check_level)
            return 0;
    } else {
        if (!(user->userlevel&arg->check_level))
            return 0;
    }
    fprintf(arg->log_file,"%-24.24s%s\n",user->userid,gen_permstr(user->userlevel,perm));
    arg->count++;
    return 0;
}
int XCheckLevel(void)
{
    struct check_level_arg arg;
    char buf[40],perm[32];
    int i;
    modify_user_mode(ADMIN);
    if (!check_systempasswd())
        return -1;
    clear();
    move(0,0); prints("\033[1;32m���ľ����ض�Ȩ�޵��û�\033[m");
    move(2,0); prints("�趨��Ҫ���ĵ�Ȩ��:");
    arg.check_level=setperms(0,0,"Ȩ��",NUMPERMS,showperminfo,NULL);
    for (arg.count=0,i=0; i<NUMPERMS; i++)
        if (arg.check_level&(1<<i))
            arg.count++;
    if (!arg.count) {
        move(2,0); clrtoeol();
        prints("δ�趨��Ҫ���ĵ�Ȩ��,��������...");
        pressreturn();
        return -1;
    }
    arg.check_mode=0;
    if (arg.count>1) {
        do {
            move(2,0); clrtoeol();
            getdata(2,0,"���趨�����Ҫ���ĵ�Ȩ��,��ѡ���߼���ϵ{��(And)|��(Or)} [A]: ",buf,2,DOECHO,NULL,true);
        } while (buf[0]&&!(buf[0]=='a'||buf[0]=='o'||buf[0]=='A'||buf[0]=='O'));
        arg.check_mode=(buf[0]=='o'||buf[0]=='O')?1:0;
    }
    sprintf(buf,"tmp/check_level_%ld_%d",time(NULL), (int)getpid());
    if (!(arg.log_file=fopen(buf,"w"))) {
        move(2,0); clrtoeol();
        prints("������ʱ�ļ�����,�����ж�...");
        pressreturn();
        return -1;
    }
    fprintf(arg.log_file,"\033[1;32m���ľ����ض�Ȩ�޵��û�����ѯ���\033[m\n\n");
    fprintf(arg.log_file,"Ȩ���趨 \033[1;33m%s\033[m <%s>\n\n",gen_permstr(arg.check_level,perm),arg.check_mode?"OR":"AND");
    arg.count=0;
    apply_users((int (*)(struct userec*,void*))check_level_func,&arg);
    fprintf(arg.log_file,"\n�� \033[1;33m%d\033[m λ�û����ϲ�ѯ����\n",arg.count);
    fclose(arg.log_file);
    move(2,0); clrtoeol();
    prints("\033[1;36m���ľ����ض�Ȩ�޵��û�����ѯ��� �ѻؼ�, �����ż�...\033[m");
    mail_file(getCurrentUser()->userid,buf,getCurrentUser()->userid,"���ľ����ض�Ȩ�޵��û�����ѯ���",BBSPOST_MOVE,NULL);
    securityreport("���ľ����ض�Ȩ�޵��û�",NULL,NULL, getSession());
    bbslog("user","%s","���ľ����ض�Ȩ�޵��û�");
    pressreturn(); clear();
    return 0;
}

#ifdef HAVE_ACTIVATION
int x_manageactivation(void)
{
    int id;
    struct userec *lookupuser;
    struct activation_info ai;
    char title[STRLEN];

    if (!HAS_PERM(getCurrentUser(), PERM_SYSOP)) {
        move(3, 0);
        clrtobot();
        prints("��Ǹ, ֻ��SYSOPȨ�޵Ĺ���Ա���ܹ����û�����");
        pressreturn();
        return 0;
    }

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return 0;
    }
    clear();
    move(0, 0);
    prints(NAME_USER_SHORT "�������\n");
    clrtoeol();
    move(1, 0);
    usercomplete("������" NAME_USER_SHORT " ID: ", genbuf);
    if (genbuf[0] == '\0') {
        clear();
        return 0;
    }
    if (!(id = getuser(genbuf, &lookupuser))) {
        move(3, 0);
        prints("�Ƿ� ID");
        clrtoeol();
        pressreturn();
        clear();
        return 0;
    }
    getactivation(&ai, lookupuser);
    move(4, 0);
    if (lookupuser->flags & ACTIVATED_FLAG) {
        prints("�û��Ѿ�����. ���� email: %s\n\n", ai.reg_email);
        if (askyn("�� Y ȡ���û�����", false)) {
            ai.activated = 0;
            setactivation(&ai, lookupuser);
            lookupuser->flags &= ~ACTIVATED_FLAG;
            sprintf(title, "%s ȡ�� %s �û�����", getCurrentUser()->userid, lookupuser->userid);
        } else {
            clear();
            return 0;
        }
    } else {
        prints("\033[1;31m�û���û�м���\033[0m\n\n");
        if (askyn("�� Y �ֶ������û�", false)) {
            ai.activated = 1;
            setactivation(&ai, lookupuser);
            lookupuser->flags |= ACTIVATED_FLAG;
            sprintf(title, "%s �ֶ����� %s �û�", getCurrentUser()->userid, lookupuser->userid);
        } else {
            clear();
            return 0;
        }
    }
    {
        FILE *fout;
        char buf[STRLEN];

        sprintf(buf, "tmp/mactivation.%s",getCurrentUser()->userid);
        if ((fout = fopen(buf, "w")) != NULL) {
            fprintf(fout, "%s\n", title);
            fprintf(fout, "�����Ǹ�������");
            getuinfo(fout, lookupuser);
            fclose(fout);
            post_file(getCurrentUser(), "", buf, "Activation", title, 0, 2, getSession());
            unlink(buf);
        }
    }
    clear();
    return 0;
}
#endif /* HAVE_ACTIVATION */

int Xdelipacl(void)
{
    int id;
    struct userec *lookupuser;

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return 0;
    }
    clear();
    move(0, 0);
    prints("ɾ��" NAME_USER_SHORT "��IP����\n");
    clrtoeol();
    move(1, 0);
    usercomplete("������" NAME_USER_SHORT " ID: ", genbuf);
    if (genbuf[0] == '\0') {
        clear();
        return 0;
    }
    if (!(id = getuser(genbuf, &lookupuser))) {
        move(3, 0);
        prints("�Ƿ� ID");
        clrtoeol();
        pressreturn();
        clear();
        return 0;
    }
    sprintf(genbuf, "home/%c/%s/ipacl", toupper(lookupuser->userid[0]), lookupuser->userid);
    unlink(genbuf);
    clear();
    return 0;
}

int Xdeljunk(void)
{
    char buf[256], board[STRLEN];

    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return 0;
    }
    clear();
    move(0, 0);
    prints("ɾ������������\n");
    clrtoeol();
    move(1, 0);
    make_blist(0, 1);
    namecomplete("��������������: ", board);
    if (board[0] == '\0') {
        clear();
        return 0;
    }
    sprintf(buf, "ȷ��ɾ�� %s ���������ͷ�ֽ¨��", board);
    if (askyn(buf, false) == false) return 0;
    sprintf(buf, "boards/%s/.DELETED", board);
    unlink(buf);
    sprintf(buf, "boards/%s/.JUNK", board);
    unlink(buf);
    sprintf(buf, "%s ��� %s ����������", getCurrentUser()->userid, board);
    securityreport(buf, NULL, NULL, getSession());
    clear();
    return 0;
}

/*�õ����˵��ղؼк�δ�ȱ��*/
int get_favread(void)
{
    char destid[IDLEN+1];
    char passwd[PASSLEN+1];
    struct userec *destuser;
    char dpath[PATHLEN];
    char mypath[PATHLEN];
    int count;

    clear();
    move(1,0);
    prints("ͬ�������ղؼ�,��������,δ����Ǻ��Զ��幦�ܼ�����ID.\n�ᵼ�±�IDԭ�����ղؼ�,��������,δ����Ǻ��Զ��幦�ܼ���ʧ������\n");
    getdata(5, 0, "������Է�ID:", destid, IDLEN+1, DOECHO, NULL, true);
    if (destid[0] == '\0' || destid[0] == '\n') {
        clear();
        return 0;
    }
    if (!getuser(destid,&destuser)) {
        move(7,0);
        prints("û������û�\n");
        pressanykey();
        return 0;
    }
    if (!strcmp(destuser->userid,getCurrentUser()->userid)) {
        move(7,0);
        prints("��Ҫ�Լ������Լ����������˵\n");
        pressanykey();
        return 0;
    }
    getdata(6, 0, "������Է�����:", passwd, PASSLEN+1, NOECHO, NULL, true);
    if (!passwd[0]) {
        move(8,0);
        prints("ȡ��...");
        pressanykey();
        return 0;
    }
    if (!checkpasswd2(passwd,destuser)) {
        logattempt(destuser->userid, getSession()->fromhost, "sync");
        move(8,0);
        prints("�������\n");
        pressanykey();
        return 0;
    }
    move(8,0);
    prints("\033[32mΪ�˱�֤����ͬ���ԣ�����ǰ�����˳���id������¼\033[m\n");
    prints("\033[31m���β����Ḳ�Ǳ�idԭ�ղؼС�����������δ����Ǻ��Զ��幦�ܼ����޷��ָ�\033[m");
    getdata(10,0,"ȷ��Ҫ���д˲�����? [y/N] ", passwd, 2, DOECHO, NULL, true);
    if (passwd[0] != 'y' && passwd[0] != 'Y') {
        clear();
        return 0;
    }

    count=0;
    getdata(12,0,"ͬ�����˶���? [Y]: ",passwd,2,DOECHO,NULL,true);
    if (toupper(passwd[0])!='N') {
        sethomefile(dpath,destuser->userid,"favboard");
        sethomefile(mypath,getCurrentUser()->userid,"favboard");
        f_cp(dpath,mypath,0);
        getSession()->mybrd_list_t=0;
        load_favboard(1,getSession());
        count++;
    }

#ifdef HAVE_BRC_CONTROL
    getdata(13,0,"ͬ��δ�����? [Y]: ",passwd,2,DOECHO,NULL,true);
    if (toupper(passwd[0])!='N') {
        sethomefile(dpath,destuser->userid,BRCFILE);
        sethomefile(mypath,getCurrentUser()->userid,BRCFILE);
        f_cp(dpath,mypath,0);

        if (getSession()->brc_cache_entry) {
            memset(getSession()->brc_cache_entry,0,BRC_CACHE_NUM*sizeof(struct _brc_cache_entry));
            brc_initial(getCurrentUser()->userid,DEFAULTBOARD,getSession());
            if (currboard)
                brc_initial(getCurrentUser()->userid,currboard->filename,getSession());
        }
        count++;
    }
#endif

    getdata(14,0,"ͬ����������? [Y]: ",passwd,2,DOECHO,NULL,true);
    if (toupper(passwd[0])!='N') {
        sethomefile(dpath,destuser->userid,"friends");
        sethomefile(mypath,getCurrentUser()->userid,"friends");
        f_cp(dpath,mypath,0);
        getfriendstr(getCurrentUser(),get_utmpent(getSession()->utmpent),getSession());
        count++;
    }

    getdata(15, 0, "ͬ���Զ��幦�ܼ�? [Y]: ", passwd, 2, DOECHO, NULL, true);
    if (toupper(passwd[0]) != 'N') {
        sethomefile(dpath, destuser -> userid, "definekey");
        sethomefile(mypath, getCurrentUser() -> userid, "definekey");
        f_cp(dpath, mypath, 0);
        load_key(NULL);
        count++;
    }

    if (count) {
        move(16,0);
        prints("�����ɹ�, ���������µ�¼����ʹ��������!");
    }

    pressanykey();
    clear();
    return 0;
}

#ifdef SECONDSITE
/*�õ���վ���ղؼк�δ�ȱ��*/
int get_mainsite(void)
{
    char passwd[PASSLEN+1];
    char dpath[PATHLEN];
    char mypath[PATHLEN];
    char cmdtmp[256];
    int count=0;
    clear();
    move(1,0);
    prints("ͬ����վ���ϵ���վ��ID.\n�ᵼ�±�վ��ID��ͬ����ԭʼ���϶�ʧ������\n");
    prints("\033[32mΪ�˱�֤����ͬ���ԣ�����ǰ�����˳���id������¼\033[m\n");
    prints("\033[31m���β����Ḳ�Ǳ�idԭ��������/�Զ�������޷��ָ�\033[m");
    getdata(10,0,"ȷ��Ҫ���д˲�����? [y/N] ", passwd, 2, DOECHO, NULL, true);
    if (passwd[0] != 'y' && passwd[0] != 'Y') {
        clear();
        return 0;
    }
    getdata(14,0,"ͬ����������? [Y]: ",passwd,2,DOECHO,NULL,true);
    if (toupper(passwd[0])!='N') {
        sprintf(dpath, "tmp/second.%s.friends", getCurrentUser()->userid);
        unlink(dpath);
        sprintf(cmdtmp, "/usr/bin/wget -O %s http://10.0.4.238:5257/home/%c/%s/friends", dpath, toupper(getCurrentUser()->userid[0]), getCurrentUser()->userid);
        system(cmdtmp);
        if (!dashf(dpath)) {
            move(14,30);
            prints("ʧ��");
            goto outfriend;
        }
        sethomefile(mypath,getCurrentUser()->userid,"friends");
        f_cp(dpath,mypath,0);
        unlink(dpath);
        getfriendstr(getCurrentUser(),get_utmpent(getSession()->utmpent),getSession());
        count++;
    }
outfriend:
    getdata(16,0,"ͬ���Զ��尴��? [Y]: ",passwd,2,DOECHO,NULL,true);
    if (toupper(passwd[0])!='N') {
        sprintf(dpath, "tmp/second.%s.definekey", getCurrentUser()->userid);
        unlink(dpath);
        sprintf(cmdtmp, "/usr/bin/wget -O %s http://10.0.4.238:5257/home/%c/%s/definekey", dpath, toupper(getCurrentUser()->userid[0]), getCurrentUser()->userid);
        system(cmdtmp);
        if (!dashf(dpath)) {
            move(16,30);
            prints("ʧ��");
            goto outkey;
        }
        sethomefile(mypath,getCurrentUser()->userid,"definekey");
        f_cp(dpath,mypath,0);
        unlink(dpath);
        load_key(NULL);
        count++;
    }
outkey:
    if (count) {
        move(20,0);
        prints("�����ɹ�, ���������µ�¼����ʹ��������!");
    }
    pressanykey();
    clear();
    return 0;
}
#endif /* SECONDSITE */


#ifdef SMS_SUPPORT
int x_usersmsdef(void)
{
    unsigned int newlevel;
    modify_user_mode(USERDEF);
    clear();
    if (!strcmp(getCurrentUser()->userid, "guest"))
        return 0;

    if (! getSession()->currentmemo->ud.mobileregistered) {
        move(3, 0);
        prints("��û��ע���ֻ�����");
        pressreturn();
        return 0;
    }
    move(2, 0);
    newlevel = setperms(getSession()->currentmemo->ud.smsdef, 0, "���Ų���", NUMSMSDEF, showsmsdef, NULL);
    move(2, 0);
    if (newlevel == getSession()->currentmemo->ud.smsdef)
        prints("����û���޸�...\n");
    else {
        getSession()->currentmemo->ud.smsdef = newlevel;
        write_userdata(getCurrentUser()->userid, &(getSession()->currentmemo->ud));
        prints("�µĲ����趨���...\n\n");
    }
    pressreturn();
    clear();
    return 0;
}
#endif

int x_userdefine1(void)
{
    int id;
    unsigned int newlevel;
    struct userec *lookupuser;

    modify_user_mode(USERDEF);
    if (!(id = getuser(getCurrentUser()->userid, &lookupuser))) {
        move(3, 0);
        prints("����� " NAME_USER_SHORT " ID...");
        clrtoeol();
        pressreturn();
        clear();
        return 0;
    }
    if (!strcmp(getCurrentUser()->userid, "guest"))
        return 0;
    move(1, 0);
    clrtobot();
    move(2, 0);
    newlevel = setperms(lookupuser->userdefine[1], 0, "����", NUMDEFINES-32, showuserdefine1, NULL);
    move(2, 0);
    if (newlevel == lookupuser->userdefine[1])
        prints("����û���޸�...\n");
    else {
        lookupuser->userdefine[1] = newlevel;
        getCurrentUser()->userdefine[1] = newlevel;
        prints("�µĲ����趨���...\n\n");
    }
    pressreturn();
    clear();
    return 0;
}

int x_userdefine(void)
{
    int id;
    unsigned int newlevel;
    extern int nettyNN;
    struct userec *lookupuser;

    modify_user_mode(USERDEF);
    if (!(id = getuser(getCurrentUser()->userid, &lookupuser))) {
        move(3, 0);
        prints("����� " NAME_USER_SHORT " ID...");
        clrtoeol();
        pressreturn();
        clear();
        return 0;
    }
    if (!strcmp(getCurrentUser()->userid, "guest"))
        return 0;
    move(1, 0);
    clrtobot();
    move(2, 0);
    newlevel = setperms(lookupuser->userdefine[0], 0, "����", 31, showuserdefine, NULL);
    move(2, 0);
    if (newlevel == lookupuser->userdefine[0])
        prints("����û���޸�...\n");
    else {
        lookupuser->userdefine[0] = newlevel;
        getCurrentUser()->userdefine[0] = newlevel;
        if (((convcode) && (newlevel & DEF_USEGB))      /* KCN,99.09.05 */
                ||((!convcode) && !(newlevel & DEF_USEGB)))
            switch_code();
        uinfo.pager |= FRIEND_PAGER;
        if (!(uinfo.pager & ALL_PAGER)) {
            if (!DEFINE(getCurrentUser(), DEF_FRIENDCALL))
                uinfo.pager &= ~FRIEND_PAGER;
        }
        uinfo.pager &= ~ALLMSG_PAGER;
        uinfo.pager &= ~FRIENDMSG_PAGER;
        if (DEFINE(getCurrentUser(), DEF_FRIENDMSG)) {
            uinfo.pager |= FRIENDMSG_PAGER;
        }
        if (DEFINE(getCurrentUser(), DEF_ALLMSG)) {
            uinfo.pager |= ALLMSG_PAGER;
            uinfo.pager |= FRIENDMSG_PAGER;
        }
        UPDATE_UTMP(pager, uinfo);
        if (DEFINE(getCurrentUser(), DEF_ACBOARD))
            nettyNN = NNread_init();
        prints("�µĲ����趨���...\n\n");
    }
    pressreturn();
    clear();
    return 0;
}

#ifdef SECONDSITE
static time_t old=0;

static void
datapipefd(int fds, int fdn)
{
    fd_set rs;
    int retv, max;
    char buf[1024];
    time_t now;

    max = 1 + ((fdn > fds) ? fdn : fds);
    FD_ZERO(&rs);
    while (1) {
        FD_SET(fds, &rs);
        FD_SET(fdn, &rs);
        retv = select(max, &rs, NULL, NULL, NULL);
        if (retv < 0) {
            if (errno != EINTR)
                break;
            continue;
        }
        if (FD_ISSET(fds, &rs)) {
#ifdef SSHBBS
            retv = ssh_read(fds, buf, sizeof(buf));
#else
            retv = read(fds, buf, sizeof(buf));
#endif
            if (retv > 0) {

                now = time(0);
                uinfo.freshtime = now;
                if (now - old > 60) {
                    UPDATE_UTMP(freshtime, uinfo);
                    old = now;
                }

                write(fdn, buf, retv);
            } else if (retv == 0 || (retv < 0 && errno != EINTR))
                break;
            FD_CLR(fds, &rs);
        }
        if (FD_ISSET(fdn, &rs)) {
            retv = read(fdn, buf, sizeof(buf));
            if (retv > 0) {
#ifdef SSHBBS
                ssh_write(fds, buf, retv);
#else
                write(fds, buf, retv);
#endif
            } else if (retv == 0 || (retv < 0 && errno != EINTR))
                break;
            FD_CLR(fdn, &rs);
        }
    }
}

void myexec_cmd(int umode,const char *cmdfile,const char *param)
{
    char param1[256];
    int save_pager;
    pid_t childpid;
    int p[2];
    param1[0] = 0;
    if (param != NULL) {
        char *avoid = "&;!`'\"|?~<>^()[]{}$\n\r\\", *ptr;
        int n = strlen(avoid);
        strncpy(param1, param, sizeof(param1));
        param1[255]='\0';
        while (n > 0) {
            n--;
            ptr = strchr(param1, avoid[n]);
            if (ptr != NULL)
                *ptr = 0;
        }
    }

    if (!dashf(cmdfile)) {
        move(2, 0);
        prints("no %s\n", cmdfile);
        pressreturn();
        return;
    }

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, p) < 0)
        return;

    modify_user_mode(umode);
    clear();
    signal(SIGALRM, SIG_IGN);
    signal(SIGCHLD, SIG_DFL);
    childpid = fork();
    if (childpid == 0) {
        close(p[0]);
        if (p[1] != 0)
            dup2(p[1], 0);
        dup2(0, 1);
        //dup2(0, 2);
        if (param1[0]) {
            execl(cmdfile, cmdfile, param1, getCurrentUser()->userid, NULL);
        } else {
            execl(cmdfile, cmdfile, getCurrentUser()->userid, NULL);
        }
        exit(0);
    } else if (childpid > 0) {
        close(p[1]);
        datapipefd(0, p[0]);
        close(p[0]);
        while (wait(NULL) != childpid)
            sleep(1);
    } else {
        close(p[0]);
        close(p[1]);
    }
    signal(SIGCHLD, SIG_IGN);

    return;
}
#endif /* SECONDSITE */

int x_cloak(void)
{
    modify_user_mode(GMENU);
    bbslog("user","%s","toggle cloak");
    uinfo.invisible = (uinfo.invisible) ? false : true;
    UPDATE_UTMP(invisible, uinfo);
    if (!uinfo.in_chat) {
        move(1, 0);
        clrtoeol();
        prints("������ (cloak) �Ѿ�%s��!", (uinfo.invisible) ? "����" : "ֹͣ");
        pressreturn();
    }
    return 0;
}

int x_date(void)
{
    time_t t;
    modify_user_mode(XMENU);
    clear();
    move(8, 0);
    time(&t);
    prints("Ŀǰϵͳ������ʱ��: %s", ctime(&t));
    clrtoeol();
    pressreturn();
    return 0;
}

int x_edits(void)
{
    int aborted;
    int gdataret;
    char ans[7], buf[STRLEN];
    int ch, num;
    char *e_file[] = { "plans", "signatures", "notes", "logout", "myurl",
#ifdef AUTOREMAIL
                       "autoremail",
#endif
                       NULL
                     };
    char *explain_file[] = { "����˵����", "ǩ����", "�Լ��ı���¼", "��վ�Ļ���", "����������ʹ�õ�����",
#ifdef AUTOREMAIL
                             "վ���ż��Զ��ظ�",
#endif
                             NULL
                           };

    modify_user_mode(GMENU);
    clear();
    move(1, 0);
    prints("���޸��˵���\n\n");
    for (num = 0; e_file[num] != NULL && explain_file[num] != NULL; num++) {
        prints("[\033[32m%d\033[m] %s\n", num + 1, explain_file[num]);
    }
    prints("[\033[32m%d\033[m] �������\n", num + 1);

    getdata(num + 5, 0, "��Ҫ������һ����˵���: ", ans, 2, DOECHO, NULL, true);
    if (ans[0] - '0' <= 0 || ans[0] - '0' > num || ans[0] == '\n' || ans[0] == '\0')
        return 0;

    ch = ans[0] - '0' - 1;

    sethomefile(genbuf, getCurrentUser()->userid, e_file[ch]);
    move(3, 0);
    clrtobot();
    sprintf(buf, "(E)�༭ (D)ɾ�� %s? [E]: ", explain_file[ch]);
    gdataret = getdata(3, 0, buf, ans, 2, DOECHO, NULL, true);
    if (gdataret == -1) return 0;
    if (ans[0] == 'D' || ans[0] == 'd') {
        my_unlink(genbuf);
        move(5, 0);
        prints("%s ��ɾ��\n", explain_file[ch]);
        sprintf(buf, "delete %s", explain_file[ch]);
        if (!strcmp(e_file[ch], "signatures")) {
            getSession()->currentmemo->ud.signum = calc_numofsig(getCurrentUser()->userid);
            write_userdata(getCurrentUser()->userid, &(getSession()->currentmemo->ud));

            if (getSession()->currentmemo->ud.signum&&(getCurrentUser()->signature==0))
                getCurrentUser()->signature=1;
        } else if (!strcmp(e_file[ch], "myurl")) {
            get_my_webdomain(-1);
        }
        bbslog("user","%s",buf);
        pressreturn();
        clear();
        return 0;
    }
    modify_user_mode(EDITUFILE);
    aborted = vedit(genbuf, false, NULL, NULL, 0);
    clear();
    if (!aborted) {
        prints("%s ���¹�\n", explain_file[ch]);
        sprintf(buf, "edit %s", explain_file[ch]);
        if (!strcmp(e_file[ch], "signatures")) {

            getSession()->currentmemo->ud.signum = calc_numofsig(getCurrentUser()->userid);
            write_userdata(getCurrentUser()->userid, &(getSession()->currentmemo->ud));

            if (getSession()->currentmemo->ud.signum&&(getCurrentUser()->signature==0))
                getCurrentUser()->signature=1;
            prints("ϵͳ�����趨�Լ��������ǩ����...");
        } else if (!strcmp(e_file[ch], "myurl")) {
            get_my_webdomain(-1);
        }
        bbslog("user","%s",buf);
    } else
        prints("%s ȡ���޸�\n", explain_file[ch]);
    pressreturn();
    return 0;
}

int a_edits(void)
{
    int aborted;
    int gdataret;
    char ans[7], buf[STRLEN];
    int ch, num;

    /*
     * Leeward 98.04.01 added: sysconf.ini
     */
    /*
     * Leeward 98.07.31 added: .badIP
     */
    /*
     * stephen 2000.10.17 added: /usr/share/apache/htdocs/script/menucontext.js
     */
    /*
     * period  2000.10.17 link /backup/www/htdocs/script/menucontext.js --> /home0/bbs/etc/www_menu.js
     */
    static const char *e_file[] = { "../Welcome", "../vote/notes", "issue", "movie", "logout", "menu.ini", "proxyIP", "mailcheck", "s_fill", "f_fill.realname", "f_fill.unit", "f_fill.address", "f_fill.telephone",
                                    "f_fill.real", "f_fill.chinese", "f_fill.toomany", "f_fill.proxy", "smail", "f_fill", "../.badname", "../.badIP", "badword", "sysconf.ini", "www_menu.js", "../0Announce/hotinfo",
                                    "../0Announce/systeminfo",
                                    "forbm",
                                    "forcloak",
                                    "forlongid", "../innd/newsfeeds.bbs", "deny_reason", "initial_favboard","tonewuser",
                                    "../" USER_TITLE_FILE,
#ifdef FLOWBANNER
                                    "banner",
#endif
#ifdef FB2KENDLINE
                                    "whatdate",
#endif
#ifdef ZIXIA
                                    "flinks.wForum",
#endif
                                    NULL
                                  };

    /*
     * "/usr/share/apache/htdocs/script/menucontext.js", NULL};
     */
    static const char *explain_file[] = { "Welcome", "���ñ���¼", "��վ��ӭ��", "�����", "��վ����", "menu.ini", "����IP", "����ȷ�ϵ�", "ע�ᵥ��ɵ�", "ע�ᵥʧ�ܵ�(��ʵ����)", "ע�ᵥʧ�ܵ�(����λ)",
                                          "ע�ᵥʧ�ܵ�(��ס��ַ)", "ע�ᵥʧ�ܵ�(����绰)", "ע�ᵥʧ�ܵ�(��ʵ����)", "ע�ᵥʧ�ܵ�(������д)", "ע�ᵥʧ�ܵ�(�����ID)", "ע�ᵥʧ�ܵ�(���ܴ���ע��)",
                                          "����ȷ����ɵ�        ",
                                          "����ȷ��ʧ�ܵ�",
                                          "����ע��� ID         ", "���ɵ�¼�� IP", "ϵͳ�Զ����˵Ĵ���    ",
                                          "sysconf.ini", "WWW���˵�             ", "�����ȵ�", "ϵͳ�ȵ�",
                                          "�����ΰ�������        ", "�������û�����",
                                          "�������û�����        ",
                                          "ת�Ű���������Ӧ",
                                          "��������б�", "���û����˶�����","����ע���û�����",
                                          "�û�ְ���",
#ifdef FLOWBANNER
                                          "ȫվ������Ϣ",
#endif
#ifdef FB2KENDLINE
                                          "������Ϣ",
#endif
#ifdef ZIXIA
                                          "web ��������",
#endif
                                          NULL
                                        };
#ifdef SOURCE_PERM_CHECK
    if (!HAS_PERM(getCurrentUser(), PERM_ADMIN)) {
        move(3, 0);
        clrtobot();
        prints("��Ǹ, ��û�� ADMIN Ȩ��!");
        pressreturn();
        return 0;
    }
#endif
    modify_user_mode(ADMIN);
    if (!check_systempasswd()) {
        return 0;
    }
    clear();
    move(0, 0);
    prints("����ϵͳ����\n\n");
    for (num = 0; e_file[num] != NULL && explain_file[num] != NULL; num++) {
        prints("[\033[32m%2d\033[m] %s%s", num + 1, explain_file[num], ((num + 1) % 2) ? "      " : "\n");
        /*
         * Leeward 98.03.29 ������ʾ���֣��Ա���롰ϵͳ�Զ����˵Ĵ��һ��
         */
        /*
         * Leeward 98.07.31 ������ʾ���֣��Ա���롰���ɵ�¼�� IP��һ��
         */
        /*
         * Bigman 2001.6.23 �������֣� ����������������
         */
    }

    prints("[\033[32m%2d\033[m] �������\n", num + 1);

    /*
     * �����21���кţ��Ժ����ӣ�������Ӧ����
     */
    getdata(21, 0, "��Ҫ������һ��ϵͳ����: ", ans, 3, DOECHO, NULL, true);
    ch = atoi(ans);
    if (!isdigit(ans[0]) || ch <= 0 || ch > num || ans[0] == '\n' || ans[0] == '\0')
        return 0;
    ch -= 1;
    sprintf(genbuf, "etc/%s", e_file[ch]);
    move(2, 0);
    clrtobot();
    sprintf(buf, "(E)�༭ (D)ɾ�� %s? [E]: ", explain_file[ch]);
    gdataret = getdata(3, 0, buf, ans, 2, DOECHO, NULL, true);
    if (gdataret == -1) return 0;
    if (ans[0] == 'D' || ans[0] == 'd') {
        {
            char secu[STRLEN];

            sprintf(secu, "ɾ��ϵͳ������%s", explain_file[ch]);
            securityreport(secu, NULL, NULL, getSession());
        }
        my_unlink(genbuf);
        move(5, 0);
        prints("%s ��ɾ��\n", explain_file[ch]);
        sprintf(buf, "delete %s", explain_file[ch]);
        bbslog("user","%s",buf);
        pressreturn();
        clear();
        return 0;
    }
    modify_user_mode(EDITSFILE);
    aborted = vedit(genbuf, false, NULL, NULL, 0);
    clear();
    if (aborted != -1) {
        prints("%s ���¹�", explain_file[ch]);
        sprintf(buf, "edit %s", explain_file[ch]);
        bbslog("user","%s",buf);
        {
            char secu[STRLEN];

            sprintf(secu, "�޸�ϵͳ������%s", explain_file[ch]);
            securityreport(secu, NULL, NULL, getSession());
        }

        if (!strcmp(e_file[ch], "../Welcome")) {
            my_unlink("Welcome.rec");
            prints("\nWelcome ��¼������");
        }
#ifdef FILTER
        if (!strcmp(e_file[ch], "badword")) {
            my_unlink(BADWORD_IMG_FILE);
            prints("\n���˴ʱ�����");
        }
#endif
        if (!strcmp(e_file[ch],"../" USER_TITLE_FILE)) {
            load_user_title();
        }
#ifdef FLOWBANNER
        if (!strcmp(e_file[ch],"banner")) {
            load_site_banner(0);
        }
#endif
#ifdef FB2KENDLINE
        if (!strcmp(e_file[ch],"whatdate")) {
            setpublicshmreadonly(0);
            publicshm->nextfreshdatetime = time(0);
            setpublicshmreadonly(0);
        }
#endif
#ifdef ZIXIA
        if (!strcmp(e_file[ch],"flinks.wForum")) {
            my_unlink("etc/flinks.wForum.html");
            prints("\n�������Ӹ���");
        }
#endif
    }
    pressreturn();
    return 0;
}

static void escape_filename(char *fn)
{
    static const char invalid[] = { '/', '\\', '!', '&', '|', '*', '?', '`', '\'', '\"', ';', '<', '>', ':' };
    int i, j;

    for (i = 0; i < strlen(fn); i++)
        for (j = 0; j < sizeof(invalid); j++)
            if (fn[i] == invalid[j])
                fn[i] = '_';
}
int zsend_file(char *filename, char *title)
{
    char buf[100], buf1[100];

    ansimore("etc/zmodem", 0);
    move(t_lines - 1, 0);
    clrtoeol();
    strcpy(buf, "N");
    getdata(t_lines - 1, 0, "��ȷ��Ҫʹ��Zmodem�����ļ�ô?[y/N]", buf, 2, DOECHO, NULL, true);
    if (toupper(buf[0]) != 'Y')
        return 1;
    strncpy(buf, title, 76);
    buf[80] = '\0';
    escape_filename(buf);
    strcat(buf, ".TXT");
    move(t_lines - 2, 0);
    clrtoeol();
    prints("�������ļ�����Ϊ�������");
    move(t_lines - 1, 0);
    clrtoeol();
    getdata(t_lines - 1, 0, "", buf, 78, DOECHO, NULL, 0);
    if (buf[0] == '\0')
        return 0;
    buf[78] = '\0';
    escape_filename(buf);
    sprintf(buf1, "SMTH-%s-", currboard->filename);
    strcat(buf1, buf);
    bbs_zsendfile(filename, buf1);
    return 0;
}

int my_inet_aton(const char * ip, struct in_addr* queryip)
{
    char* p1;
    unsigned int i1,i2,i3,i4;
    unsigned char i[4];

    p1=(char*)ip;
    i1=atoi(p1);
    if (i1>255) return 0;
    i[0]=i1;
    while (*p1&&*p1!='.') {
        if (!isdigit(*p1))
            return 0;
        p1++;
    }
    if (!*p1) return 0;
    p1++;

    i2=atoi(p1);
    i[1]=i2;
    if (i2>255) return 0;
    while (*p1&&*p1!='.') {
        if (!isdigit(*p1))
            return 0;
        p1++;
    }
    if (!*p1) return 0;
    p1++;

    i3=atoi(p1);
    if (i3>255) return 0;
    i[2]=i3;
    while (*p1&&*p1!='.') {
        if (!isdigit(*p1))
            return 0;
        p1++;
    }
    if (!*p1) return 0;
    p1++;

    i4=atoi(p1);
    if (i4>255) return 0;
    i[3]=i4;
    while (*p1) {
        if (!isdigit(*p1))
            return 0;
        p1++;
    }
    if (!*p1) {
        memcpy(queryip,i,4);
        return 1;
    }

    return 0;
}

/* etnlegend, 2006.12.13, ��ѯ IP ��ַ�ĵ���λ�ø���, ֱ��ʹ�� QQWry.Dat ��Ϣ��... */
static inline int qqwry_read_int(int fd,uint32_t *data,size_t count,off_t offset)
{
    uint8_t buf[4]={0,0,0,0};
    if (pread(fd,buf,count,offset)!=count)
        return -1;
    *data=(buf[0]|(buf[1]<<8)|(buf[2]<<16)|(buf[3]<<24));
    return 0;
}

static inline int qqwry_read_str(int fd,char *data,size_t count,off_t offset)
{
    if (!(pread(fd,data,count,offset)>0))
        return -1;
    data[count-1]=0;
    return 0;
}

int qqwry_search(char *description,const char *ip_address)
{
#define QQWRY_DAT           "etc/QQWry.Dat"
#define QQWRY_QUIT(r)       do{close(fd);return (r);}while(0)
#define QQWRY_READ_INT(s,o) do{if(qqwry_read_int(fd,&data,(s),(o))==-1)QQWRY_QUIT(-1);}while(0)
#define QQWRY_READ_STR(s,o) do{if(qqwry_read_str(fd,buf,(s),(o))==-1)QQWRY_QUIT(-1);size=(strlen(buf)+1);}while(0)
    struct in_addr sin;
    char buf[STRLEN],*separator;
    int fd,match,bound_b,bound_e,current;
    uint32_t data,ip,ip_b,ip_e;
    off_t index_b,index_e,index_offset,record_offset,country_offset,location_offset;
    size_t size;
    if ((fd=open(QQWRY_DAT,O_RDONLY,0644))==-1)
        QQWRY_QUIT(-1);
    if (!inet_aton(ip_address,&sin))
        QQWRY_QUIT(-1);
    ip=ntohl(sin.s_addr);
    QQWRY_READ_INT(4,0);
    index_b=(off_t)data;
    QQWRY_READ_INT(4,4);
    index_e=(off_t)data;
    match=0;
    bound_b=0;
    bound_e=(index_e-index_b)/7;
    while (bound_b<bound_e) {
        current=(bound_b+bound_e)/2;
        index_offset=(index_b+current*7);
        QQWRY_READ_INT(4,index_offset);
        ip_b=(uint32_t)data;
        QQWRY_READ_INT(3,index_offset+4);
        record_offset=(off_t)data;
        QQWRY_READ_INT(4,record_offset);
        ip_e=(uint32_t)data;
        if (ip<ip_b)
            bound_e=current;
        else if (ip>ip_e)
            bound_b=current+1;
        else {
            match=1;
            break;
        }
    }
    if (!match)
        QQWRY_QUIT(0);
    QQWRY_READ_INT(1,record_offset+4);
    if (data==0x01) {
        QQWRY_READ_INT(3,record_offset+5);
        country_offset=(off_t)data;
        QQWRY_READ_INT(1,country_offset);
        if (data==0x02) {
            location_offset=country_offset+4;
            QQWRY_READ_INT(3,country_offset+1);
            country_offset=(off_t)data;
            QQWRY_READ_STR(STRLEN,country_offset);
        } else {
            QQWRY_READ_STR(STRLEN,country_offset);
            location_offset=country_offset+size;
        }
    } else if (data==0x02) {
        location_offset=record_offset+8;
        QQWRY_READ_INT(3,record_offset+5);
        country_offset=(off_t)data;
        QQWRY_READ_STR(STRLEN,country_offset);
    } else {
        QQWRY_READ_STR(STRLEN,record_offset+4);
        location_offset=(record_offset+4)+size;
    }
    strcpy(description,buf);
    if (size>(STRLEN-4))
        QQWRY_QUIT(1);
    separator=&description[size];
    separator[-1]=' ';
    QQWRY_READ_INT(1,location_offset);
    if (data==0x01||data==0x02) {
        QQWRY_READ_INT(3,location_offset+1);
        location_offset=(off_t)data;
        if (!location_offset)
            buf[0]=0;
        else
            QQWRY_READ_STR(STRLEN-size,location_offset);
    } else
        QQWRY_READ_STR(STRLEN-size,location_offset);
    strcpy(separator,buf);
    QQWRY_QUIT(1);
#undef QQWRY_DAT
#undef QQWRY_QUIT
#undef QQWRY_READ_INT
#undef QQWRY_READ_STR
}

int search_ip(void)
{
    char ip_address[16],description[STRLEN];
    int ret;
    clear();
    move(0,0);
    prints("\033[1;32m%s\033[m","[��ѯ IP ��ַλ��]");
    do {
        move(1,0);
        clrtobot();
        getdata(2,0,"��������Ҫ��ѯ�� IP ��ַ: ",ip_address,16,DOECHO,NULL,true);
        if (!ip_address[0])
            break;
        ret=qqwry_search(description,ip_address);
        move(4,0);
        if (ret==-1)
            prints("\033[1;31m%s\033[0;33m<Enter>\033[m","��ѯ�����з�������...");
        else if (!ret)
            prints("\033[1;36m%s\033[0;33m<Enter>\033[m","��ǰ���ݿ���û����� IP ��ַ��ص�λ����Ϣ...");
        else
            prints("\033[1;33m<%s> [%s]\033[m\n\n\033[1;32m��ѯ�ɹ�!\033[0;33m<Enter>\033[m",ip_address,description);
        WAIT_RETURN;
    } while (1);
    return 0;
}
/* END - etnlegend, 2006.12.13, ��ѯ IP ��ַ�ĵ���λ�ø���, ֱ��ʹ�� QQWry.Dat ��Ϣ��... */

int kick_all_user(void)
{
    struct user_info *uin;
    char ans[3];
    int i;

    if (!check_systempasswd()) {
        return -1;
    }
    clear();
    move(3,0);
    prints("��������:\n");
    prints("    ���ȹر� bbsd, sshbbsd, httpd");
    getdata(7, 0, "ȷ��Ҫ�ߵ����������û�? (y/N) [N]:", ans, 3, DOECHO, NULL, true);
    if (ans[0] != 'y' && ans[0] != 'Y')
        return 0;

    for (i=0; i < USHM_SIZE ; i++) {
        uin = get_utmpent(i);
        if (!uin || !uin->uid)
            continue;
        move(10,0);
        clrtoeol();
        prints("������  %s\n",uin->userid);
        if (uin->pid == uinfo.pid)
            continue;
        kick_user_utmp(uin->uid, uin, 0);
    }

    move(13,0);
    prints("������\n");
    pressanykey();
    return 1;
}

static int perm_mailback(struct fileheader *fh)
{
    if (!isowner(getCurrentUser(), fh)) {
        return 0;
    } else
        return 1;
}
static int set_mailback(struct fileheader *fh, int i)
{
    if (i==0) {
        fh->accessed[1] &= ~FILE_MAILBACK;
    } else {
        fh->accessed[1] |= FILE_MAILBACK;
    }
    return 1;
}
static int get_mailback(struct fileheader *fh)
{
    if (fh->accessed[1] & FILE_MAILBACK)
        return 1;
    else
        return 0;
}

static int perm_innflag(struct fileheader *fh)
{
    if (!(currboard->flag & BOARD_OUTFLAG)) return 0;
    if (isowner(getCurrentUser(), fh)) return 1;
    return chk_currBM(currboard->BM,getCurrentUser());
}
static int set_innflag(struct fileheader *fh,int arg)
{
    if (!arg) {
        fh->innflag[0]='L';
        fh->innflag[1]='L';
    } else {
        fh->innflag[0]='S';
        fh->innflag[1]='S';
    }
    return 1;
}
static int get_innflag(struct fileheader *fh)
{
    return (fh->innflag[0]==fh->innflag[1]&&fh->innflag[0]=='S')?1:0;
}

static int perm_cancelo(struct fileheader *fh)
{
    //fancy Oct 24 2008
    //if(!(fh->accessed[0] & FILE_IMPORTED)) return 0;
    return chk_currBM(currboard->BM,getCurrentUser());
}
static int set_cancelo(struct fileheader *fh, int i)
{
    if (i==0) {
        fh->accessed[0] &= ~FILE_IMPORTED;
    } else {
        fh->accessed[0] |= FILE_IMPORTED;
    }
    return 1;
}
static int get_cancelo(struct fileheader *fh)
{
    if (fh->accessed[0] & FILE_IMPORTED)
        return 1;
    else
        return 0;
}

static int perm_tex(struct fileheader *fh)
{
    if (isowner(getCurrentUser(), fh)) return 1;
    return chk_currBM(currboard->BM,getCurrentUser());
}
static int set_tex(struct fileheader *fh, int i)
{
    if (i==0) {
        fh->accessed[1] &= ~FILE_TEX;
    } else {
        fh->accessed[1] |= FILE_TEX;
    }
    return 1;
}
static int get_tex(struct fileheader *fh)
{
    if (fh->accessed[1] & FILE_TEX)
        return 1;
    else
        return 0;
}

static int perm_total(struct fileheader *fh)
{
    return chk_currBM(currboard->BM,getCurrentUser());
}
static int set_total(struct fileheader *fh, int i)
{
    if (i==0) {
        fh->accessed[0] &= ~FILE_TOTAL;
    } else {
        fh->accessed[0] |= FILE_TOTAL;
    }
    return 1;
}
static int get_total(struct fileheader *fh)
{
    if (fh->accessed[0] & FILE_TOTAL)
        return 1;
    else
        return 0;
}
#define FH_SELECT_NUM 5
static struct _fh_select {
    char *desc;
    int (*have_perm)(struct fileheader *);
    int (*set)(struct fileheader *, int);
    int (*get)(struct fileheader *);
} fh_select[FH_SELECT_NUM] = {
    {"����ת�ĵ�����", perm_mailback, set_mailback, get_mailback},
    {"ת�ŷ���", perm_innflag, set_innflag, get_innflag},
    {"�վ������", perm_cancelo, set_cancelo, get_cancelo},
    {"TeX���", perm_tex, set_tex, get_tex},
    {"�����ϼ����", perm_total, set_total, get_total}
};

int show_fhselect(struct _select_def *conf, int i)
{
    struct _setperm_select *arg = (struct _setperm_select *) conf->arg;

    i = i - 1;
    if (i == conf->item_count - 1) {
        prints("%c. �˳� ", 'A' + i);
    } else if (!(arg->basic & (1 << i))) {
        prints("%c. \033[30;1m%-27s:DENY\033[m", 'A' + i, fh_select[i].desc);
    } else if ((arg->pbits & (1 << i)) != (arg->oldbits & (1 << i))) {
        prints("%c. %-27s \033[31;1m%3s\033[m", 'A' + i, fh_select[i].desc, ((arg->pbits >> i) & 1 ? "ON" : "OFF"));
    } else {
        prints("%c. %-27s \x1b[37;0m%3s\x1b[m", 'A' + i, fh_select[i].desc, ((arg->pbits >> i) & 1 ? "ON" : "OFF"));
    }
    return SHOW_CONTINUE;
}

int fhselect_select(struct _select_def *conf)
{
    struct _setperm_select *arg = (struct _setperm_select *) conf->arg;

    if (conf->pos == conf->item_count)
        return SHOW_QUIT;
    if (!(arg->basic & (1 << (conf->pos - 1)))) {
        return SHOW_CONTINUE;
    }
    arg->pbits ^= (1 << (conf->pos - 1));
    return SHOW_REFRESHSELECT;
}

#include "read.h"

int fhselect(struct _select_def* conf,struct fileheader *fh,long flag)
{
    int i;
    unsigned int oldlevel=0;
    unsigned int perms=0;
    unsigned int newlevel;
    int oldmode;
    struct write_dir_arg dirarg;
    struct read_arg* arg=(struct read_arg*)conf->arg;
    struct fileheader *originFh;

    if (arg->mode!=DIR_MODE_NORMAL) {
        return DONOTHING;
    }
    /*
    if (!HAS_PERM(getCurrentUser(), PERM_SYSOP))
           return DONOTHING;
       */
    oldmode = uinfo.mode;
    modify_user_mode(USERDEF);

    for (i=0; i<FH_SELECT_NUM; i++) {
        if (fh_select[i].have_perm==NULL || fh_select[i].have_perm(fh)) {
            perms |= (1<<i);
            if (fh_select[i].get(fh)) {
                oldlevel |= (1<<i);
            }
        }
    }

    move(1, 0);
    clrtobot();
    move(2, 0);
    prints("\033[1;31m�޸�\033[1;32m%s\033[1;31m����������:\033[m \033[1;33m%s\033[m", fh->owner, fh->title);
    newlevel = setperms(oldlevel, perms, "����", FH_SELECT_NUM, show_fhselect, fhselect_select);
    move(22, 0);
    if ((newlevel & perms) == (oldlevel & perms))
        prints("����û���޸�...\n");
    else {
        for (i=0; i<FH_SELECT_NUM; i++) {
            if ((perms & (1<<i)) && ((oldlevel & (1<<i)) != (newlevel & (1<<i)))) {
                fh_select[i].set(fh, newlevel & (1<<i));
                if (i == 3) {
                    char bfile[PATHLEN];
                    setbfile(bfile, currboard->filename, fh->filename);
                    if (dashf(bfile))
                        f_touch(bfile);
                }
            }
        }

        init_write_dir_arg(&dirarg);
        dirarg.fd=arg->fd;
        dirarg.ent = conf->pos;
        if (prepare_write_dir(&dirarg, fh, arg->mode) == 0) {
            originFh = dirarg.fileptr + (dirarg.ent - 1);
            memcpy(originFh, fh, sizeof(struct fileheader));

            if (dirarg.needlock)
                un_lock(dirarg.fd, 0, SEEK_SET, 0);

            free_write_dir_arg(&dirarg);
            prints("�µĲ����趨���...\n\n");
        } else {
            free_write_dir_arg(&dirarg);
            prints("ϵͳ����ʧ��...\n");
        }

    }
    pressreturn();
    modify_user_mode(oldmode);
    return FULLUPDATE;
}

/*
 *  Term�¹����Ƽ�����, jiangjun, 2007.11.02
 */

#define MAXRCMDBRD 15
/* ע��, ��� show_rcmd_brd() û�з�ҳ����, �������̫���� ... fancy Nov 5 2007 */
#define RCMDBOARD  "xml/rcmdbrd.xml"

void show_rcmd_brd(struct boardheader bh[], int total)
{
    int i=0;
    char buf[STRLEN];

    move(3, 0);
    clrtobot();
    prints("Ŀǰ������ \033[1;32m%d\033[m ���Ƽ�����:", total);
    while (i<total) {
        if (bh[i].filename[0]==0)
            break;
        move(i+4, 0);
        sprintf(buf, "\033[1;3%dm%-20s\t%-20s\033[m", i%2?2:3, bh[i].filename, bh[i].title+13);
        prints(buf);
        i++;
    }
}

int set_rcmdbrd()
{
    char buf[256],buf1[256], *ptr, ans[2];
    FILE *fp;
    int i, total, bid;
    struct boardheader bh[MAXRCMDBRD], *newbh;

    total = 0;

    if (!HAS_PERM(getCurrentUser(), PERM_SYSOP))
        return DONOTHING;

    newbh = (struct boardheader *) malloc(sizeof(struct boardheader));
    memset(&bh, 0, MAXRCMDBRD * sizeof(struct boardheader));
    /* get old recommend board data */
    if ((fp=fopen(RCMDBOARD, "r"))!=NULL) {
        while (fgets(buf, 256, fp)!=NULL) {
            if ((ptr=strstr(buf, "EnglishName"))!=NULL) {
                strcpy(buf1, ptr+strlen("EnglishName>"));
                if ((ptr=strchr(buf1, '<'))!=NULL)
                    *ptr = '\0';
                if (getboardnum(buf1, &(bh[total]))==0)
                    continue;
                total++;
            }
            if (total>=MAXRCMDBRD)
                break;
        }
        fclose(fp);
    }

    while (1) {
        clear();
        move(0, 0);
        prints("�Ƽ�������ѡ��");
        /*Here:  // board exists while adding or deleteing, should goto here*/
        show_rcmd_brd(bh, total);
        getdata(1, 0, "(A)���������� (D)ɾ�������� (E)�˳� [E]: ", ans, 2, DOECHO, NULL, true);
        if (ans[0]=='a' || ans[0]=='A') {
            if (total>=MAXRCMDBRD) {
                move(2, 0);
                prints("�Ѿ��ﵽ����Ƽ���������Ŀ!");
                WAIT_RETURN;
                continue;
            }
            make_blist(0, 1);
            move(1, 0);
            namecomplete("�����������ӵ�������Ӣ����: ", buf);
            if (!buf[0])
                continue;
            bid=getboardnum(buf, newbh);
            if (bid==0) {
                move(2, 0);
                prints("����������!");
                WAIT_RETURN;
                continue;
            }
            if (!public_board(newbh)) {
                move(2, 0);
                prints("�����Ƽ��ڲ�������!");
                WAIT_RETURN;
                continue;
            }
            for (i=0; i<total; i++) {
                if (!strcasecmp(buf, bh[i].filename)) {
                    move(2, 0);
                    prints("�Ѿ����Ӹ�������!");
                    WAIT_RETURN;
                    /*goto Here;*/
                    break;
                }
            }
            if (i < total) continue;
            memcpy(&(bh[total]), newbh, sizeof(struct boardheader));
            total ++;
        } else if (ans[0]=='d' || ans[0]=='D') {
            getdata(1, 0, "��������ɾ����������Ӣ����: ", buf, STRLEN, DOECHO, NULL, true);
            if (buf[0]) {
                for (i=0; i<total; i++) {
                    if (!strcasecmp(buf, bh[i].filename)) {
                        /*bh[i].filename[0]='\0';
                        memmove(&(bh[i]), &(bh[i+1]), (total - i - 1) * sizeof(struct boardheader));
                        memset(&(bh[total-1]), 0, sizeof(struct boardheader));
                        total --;
                        move(2, 0);
                        prints("�Ѿ�ɾ��������");
                        WAIT_RETURN;
                        goto Here;*/
                        break;
                    }
                }
                if (i < total) {
                    bh[i].filename[0]='\0';
                    memmove(&(bh[i]), &bh[i+1], (total - i - 1) * sizeof(struct boardheader));
                    memset(&(bh[total-1]), 0, sizeof(struct boardheader));
                    total --;
                    move(2, 0);
                    prints("�Ѿ�ɾ��������");
                    WAIT_RETURN;
                    continue;
                }
                move(2, 0);
                prints("�����������б���");
                WAIT_RETURN;
            }
        } else
            break;
    }

    free(newbh);
    if ((fp=fopen(RCMDBOARD, "w"))==NULL) {
        move(2, 0);
        prints("ϵͳ����!");
        WAIT_RETURN;
        return FULLUPDATE;
    }
    fprintf(fp, "<?xml version=\"1.0\" encoding=\"GBK\"?>\n"
            "<RecommendBoards>\n");
    for (i=0; i<total; i++) {
        fprintf(fp, "<Board><EnglishName>%s</EnglishName></Board>\n", bh[i].filename);
    }
    fprintf(fp, "</RecommendBoards>\n");
    fclose(fp);
    return FULLUPDATE;
}
