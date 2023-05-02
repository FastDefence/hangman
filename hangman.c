#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
  
char dummy;

typedef struct DISP{
  int flag;
  char resdisp;
}disp;
typedef struct WORD_PLAYED{
  char word[51];
  int clear;
  int fail;
}word_played;

void hangman_game(word_played data[]);//ハングマンの
void dispwordlist(word_played data[]);//プレイした単語のリストを表示
char select_word();//ファイル等から単語選択

int main(){
  word_played w[1500];
  for(int i=0;i<1500;i++){
    strcpy(w[i].word,"#");
    w[i].clear=0;
    w[i].fail=0;  
  }

  //プロンプト
  while(1){
    char option; 
    printf("$オプションを選択\n");
    printf("$C:続ける,始める Q:やめる L:出てきたワードのリストとその詳細\n");
    printf("$>");
    scanf("%c%c",&option,&dummy);

    if(option=='C'||option=='c'){
      hangman_game(w);
    }
    if(option=='Q'||option=='q'){
      return 0;
    }
    if(option=='L'||option=='l'){
      dispwordlist(w);
    }
    if(option=='R'||option=='r'){
      srand((unsigned)time(NULL));
      select_word();
    }
  }
}


void hangman_game(word_played data[]){  
  int inputed[26];  
  int tried;
  char correct[51];
  int correct_len;
  int remain;

  memset(inputed,0,sizeof(inputed));
  tried=0;
  printf("$単語を入力>");
  scanf("%s%c",correct,&dummy);//後でファイル読み込みの処理にかえる
  correct_len=strlen(correct);
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

  //お手付き７回か、正解が出るまで繰り返し
  while(tried<7&&remain>0){
    char key;
    int charflag=0;
    printf("$単語の文字を入力してください>");
    scanf("%c%c",&key,&dummy);

    //文字照合
    for(int i=0;i<correct_len;i++){
      if(key==d[i].resdisp){
        d[i].flag=1;
        charflag=1;
        if(inputed[key-'a']==0){
          remain--;
        }
      }
    }

    //お手付き時,使用された文字の確認
    if(charflag==0){
      if(inputed[key-'a']==0){
        tried++;
        printf("$お手付き！お手付き回数はあと%d回です\n",7-tried);
      }
    }
    inputed[key-'a']++;

    //結果の表示
    printf("$");
    for(int i=0;i<correct_len;i++){
      if(d[i].flag==1){
        printf("%c",d[i].resdisp);
      }
      else{printf("-");}
    }
    printf("\n");

    //使用した単語の表示
    printf("$使用した単語:");
    for(int i=0;i<26;i++){
      if(inputed[i]>0){
        printf("%c ",'a'+i);
      }
    }
    printf("\n");
  }

  //正解時と不正解時の処理
  if(tried>=7){
    printf("Game Over\n");
    data[var].fail++;
  }
  if(remain==0){
    printf("Game Clear\n");
    data[var].clear++;
  }
}

void dispwordlist(word_played data[]){
  for(int i=0;strcmp(data[i].word,"#")!=0;i++){
    printf("%s:正解%d回 不正解%d回\n",data[i].word,data[i].clear,data[i].fail);
  }
}

char select_word(){
  FILE *fp;
  int filesize;
  int filepos;
  char sentence[151];
  char words[51];
  char c;//fget用
  int count=0;

  //ファイル読み込み、サイズ取得、ファイルのランダムな場所にシーク
  fp = fopen("toeic1500_utf.dat","rb");
  fseek(fp,0,SEEK_END);
  filesize=ftell(fp);
  filepos=rand()%filesize;
  fseek(fp,filepos,SEEK_SET);

  //単語探索　行頭にシーク→行読み込み
  if((float)filepos / filesize < 0.5){//シーク場所がテキストファイルの前半の時の処理
    while(((c=fgetc(fp)) != '\n')){
      filepos++;
      fseek(fp,filepos,SEEK_SET);
    }
    while(((c=fgetc(fp)) != '\n')){
      filepos++;
      fseek(fp,filepos,SEEK_SET);
      sentence[count]=(char)fgetc(fp);
      count++;
    }
  } 
  else{//シーク場所がテキストファイルの後半の時の処理
    while((c=fgetc(fp)) != '\n'){
      filepos--;
      fseek(fp,filepos,SEEK_SET);
    }
    while(((c=fgetc(fp)) != '\n')){
      filepos++;
      fseek(fp,filepos,SEEK_SET);
      sentence[count]=(char)fgetc(fp);
      count++;
    }
  }
  printf("%s\n",words);

  fclose(fp);
}