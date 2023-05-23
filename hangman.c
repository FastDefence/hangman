#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
  
char dummy;
char option;//選択肢入力

typedef struct DISP{//ゲーム画面表示用
  int flag;
  char resdisp;
}disp;
typedef struct WORD_PLAYED{//プレイした単語とその成績の詳細
  char word[51];
  int clear;
  int fail;
}word_played;
typedef struct STATS{//総合成績
  int s_clear;
  int s_fail;
}stats;

void hangman_game(word_played data[],stats s[]);/*ハングマンのゲーム部分*/
char* select_word();/*辞書ファイルから単語を選択する*/
void dispresult(word_played data[],stats s[]);/*プレイした単語のリストとその詳細、総合成績を表示(正解数、不正解数、正解率)*/
void filewrite(char word[]);/*間違えた単語の辞書ファイル書き込み*/

int main(){
  word_played w[1500];
  for(int i=0;i<1500;i++){
    strcpy(w[i].word,"#");
    w[i].clear=0;
    w[i].fail=0;  
  }
  stats s[1];
  s[0].s_clear=0;
  s[0].s_fail=0;

  /*プロンプト*/
  while(1){
    printf("$オプションを選択\n");
    printf("$C:ゲームを始める Q:やめる L:出てきたワードのリストとその詳細\n");
    printf("$>>");
    scanf("%c",&option);
    scanf("%c",&dummy);
    if(option=='C'||option=='c'){
      hangman_game(w,s);
    }
    if(option=='Q'||option=='q'){
      dispresult(w,s);
      printf("----バイバイ----\n");
      return 0;
    }
    if(option=='L'||option=='l'){
      dispresult(w,s);
    }
  }

  return 0;
}

void hangman_game(word_played data[],stats s[]){  
  int flag=0;
  int inputed[26];//入力された単語の集合  
  char correct[51];//正解の単語
  int correct_len;//正解の単語の文字数(長さ)
  int tried;//お手付き回数
  int remain;//当てられていない文字数

  memset(inputed,0,sizeof(inputed));
  printf("$苦手克服モードでプレイしますか？(y=はい,n=いいえ)\n");
  printf("$>>");
  scanf("%c%c",&option,&dummy);  
  while(flag==0){
    if(option=='y'||option=='Y'){
      char filename[50]="incorrect.dat";
      strcpy(correct,select_word(filename));
      flag++;
    }
    else if(option=='n'||option=='N'){
      char filename[50]="toeic1500_utf.dat";
      strcpy(correct,select_word(filename));
      flag++;
    }
  }
  correct_len=strlen(correct);
  tried=0;
  remain=correct_len;

  disp d[correct_len];
  for(int i=0;i<correct_len;i++){
    d[i].resdisp=correct[i];
    d[i].flag=0;
  }
  int var;
  for(var=0;(data[var].clear!=0||data[var].fail!=0)&&strcmp(data[var].word,correct)!=0;var++){
  }
  strcpy(data[var].word,correct);

  /*お手付き７回か、正解が出るまで繰り返し*/
  while(tried<7&&remain>0){
    char key;
    int charflag=0;

    /*結果の表示*/
    printf("$");
    for(int i=0;i<correct_len;i++){
      if(d[i].flag==1){
        printf("%c",d[i].resdisp);
      }
      else{
        printf("-");
      }
    }
    printf("\n");
    printf("$単語の文字を入力してください>>");
    scanf("%c%c",&key,&dummy);
    /*文字照合*/
    for(int i=0;i<correct_len;i++){
      if(key==d[i].resdisp){
        d[i].flag=1;
        charflag=1;
        if(inputed[key-'a']==0){
          remain--;
        }
      }
    }
    /*お手付き時,使用された文字の確認*/
    if(charflag==0){
      if(inputed[key-'a']==0){
        tried++;
        printf("$お手付き！お手付き回数はあと%d回です\n",7-tried);
      }
    }
    inputed[key-'a']++;
    /*同じ文字が複数回入力された場合*/
    if(inputed[key-'a']>1){
      printf("$その文字はもう入力されています\n");
    }
    /*使用した単語の表示*/
    printf("$使用した単語:");
    for(int i=0;i<26;i++){
      if(inputed[i]>0){
        printf("%c ",'a'+i);
      }
    }
    printf("\n\n");
  }
  /*正解時と不正解時の処理*/
  if(tried>=7){
    printf("=====Game Over=====\n");
    printf(" 正解は%sでした！\n\n",correct);
    data[var].fail++;
    s[0].s_fail++;
    filewrite(correct);
  }
  if(remain==0){
    printf("=====Game Clear=====\n");
    printf(" 正解は%sでした！\n\n",correct);
    data[var].clear++;
    s[0].s_clear++;
  }
}

char* select_word(char filename[]){
  FILE *fp;//ファイルポインタ
  int filesize;//ファイルの大きさ
  int filepos;//ファイルポインタの場所
  char sentence[151];//抜き出した行を格納する文字列配列
  char words[51];//抜き出した英単語を格納する文字列配列
  int count=0;//sentenceやwordのイテレータ
  char c;//ファイル中の文字入力用

  fp = fopen(filename,"rb");
  fseek(fp,0,SEEK_END);
  filesize=ftell(fp);
  filepos=1000;
  srand((unsigned)time(NULL));
  filepos=rand()%filesize+1;
  fseek(fp,filepos,SEEK_SET);

  /*行抜出し*/
  /*シーク場所がテキストファイルの前半の時の処理*/
  if((float)filepos / filesize < 0.5){
    while(((c=fgetc(fp))!='\n')){
      filepos++;
      fseek(fp,filepos,SEEK_SET);
    }
    while(((c=fgetc(fp))!='\n')){
      filepos++;
      fseek(fp,filepos,SEEK_SET);
      sentence[count]=(char)fgetc(fp);
      count++;
    }
  }
  /*シーク場所がテキストファイルの後半の時の処理*/
  else{
    while((c=fgetc(fp))!='\n'){
      filepos--;
      fseek(fp,filepos,SEEK_SET);
    }
    while(((c=fgetc(fp))!='\n')){
      filepos++;
      fseek(fp,filepos,SEEK_SET);
      sentence[count]=(char)fgetc(fp);
      count++;
    }
  }
  /*英単語抜出し*/
  count=0;
  for(int i=0;sentence[i]!='\0';i++){
    if((sentence[i]>='a'&&sentence[i]<='z')||(sentence[i]>='A'&&sentence[i]<='Z')){
      words[count]=sentence[i];
      count++;
    }
    else if(count>0){
      break;
    }
  }
  words[count]='\0';
  /*返す文字列変数の作成*/
  char* words_return=NULL;
  words_return=(char*)malloc(sizeof(char)*strlen(words));
  strcpy(words_return,words);
  fclose(fp);

  return words_return;
}

void dispresult(word_played data[],stats s[]){
  printf("$RESULT\n");
  for(int i=0;strcmp(data[i].word,"#")!=0;i++){
    double word_ratio=(double)(data[i].clear)/(double)(data[i].clear+data[i].fail);//単語ごとの正解率
    printf("$%s:正解%d回 不正解%d回 ",data[i].word,data[i].clear,data[i].fail);
    printf("$正答率は%d%%です\n",(int)(word_ratio*100)); 
  }
  double total_ratio=(double)(s[0].s_clear)/(double)(s[0].s_clear+s[0].s_fail);//総合正解率
  printf("$総合:正解%d回 不正解%d回 ",s[0].s_clear,s[0].s_fail);
  printf("$総合正答率は%d%%です\n",(int)(total_ratio*100));
}

void filewrite(char word[]){
  FILE* fp;
  fp=fopen("incorrect.dat","a");
  fprintf(fp,"%s\n",word);
  fclose(fp);
}