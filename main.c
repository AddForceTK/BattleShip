#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 5
#define COLUMNS 5
#define SHIPMAXNUMBER 1		//プレイヤーの最大所持艦数
#define ENEMYSHIPNUMBER 1   //敵の最大戦艦所持数
#define SHIPNAMENUMBER 20	//戦艦の最大名前文字
#define NOTCOMPLETED -1		//まだ戦艦がいる
#define COMPLETED 0			//戦艦をすべて破壊した
#define NAMEOFFSET 1        //戦艦を表示するためのオフセット(mapmarkの地形の数)
#define NOSHIP -1			//破壊されたときのパラメータ（mapの何もないと同値）

enum shoot {MISS, NEAR, HIT};
enum life {SUBMARINE=1, DESTROYER, WARSHIP};

typedef struct{
	int position_x;						//戦艦のX座標
	int position_y;						//戦艦のY座標
	int life;							//戦艦のHP
}BattleShip;

typedef struct{
	int map[ROWS][COLUMNS];				//すべてのフィールド情報
	int shipname[SHIPMAXNUMBER];		//所持している戦艦の種類
	//int shiplife[SHIPMAXNUMBER];		//所持している戦艦のHP
	BattleShip battleship[SHIPMAXNUMBER];	//所持している戦艦のステータス
}Status;

char shipfullname[][SHIPNAMENUMBER]={"Submarine", "Destroyer", "WarShip"};
char shipmark[]={'S', 'D', 'W'};							//shipname用
char mapmark[][SHIPNAMENUMBER]={"□", "Ｓ", "Ｄ", "Ｗ"};	//map用

/*mapのステータスをすべて-1（何もなし）にする*/
void InitialMAP(Status *ship){
	for(int i=0; i<COLUMNS; i++){
		for(int j=0; j<ROWS; j++){
			ship->map[i][j]=NOSHIP;
		}
	}
}

/*マップの表示(数値)*/
void PrintMapINT(Status *ship){
	for(int y=0; y<COLUMNS; y++){
		for(int x=0; x<ROWS; x++){
			printf("%2d ", ship->map[y][x]);
		}
		putchar('\n');
	}
}

/*マップの表示(マーク)*/
void PrintMapMark(Status *ship){
	puts("　　Ｘ座標→");
	puts("　　０１２３４");
	for(int y=0; y<COLUMNS; y++){
		if(y==0){
			printf("Ｙ０");
		}else if(y==1){
			printf("↓１");
		}else if(y==2){
			printf("座２");
		}else if(y==3){
			printf("表３");
		}else if(y==4){
			printf("　４");
		}

		for(int x=0; x<ROWS; x++){
			printf("%s", mapmark[ship->map[y][x]+NAMEOFFSET]);
		}
		putchar('\n');
	}
}

/*戦艦の初期位置を指定*/
void ShipInitialPosition(Status *ship, int n){
	int x, y;
	int flag=0;
	do{
//		PrintMapINT(ship);		//数値で表示
		system("cls");
		puts("Player Field");
		PrintMapMark(ship);     //マークで表示
		printf("%sの初期位置を選択して下さい。\n", shipfullname[ship->shipname[n]]);

		printf("X座標:"); fflush(0); scanf("%d", &x);
		if((x>=0) && (x<=ROWS-1)){
			flag=1;
		}else{
			puts("座標が正しくありません。");
			flag=0;
			continue;
		}

		printf("Y座標:"); fflush(0); scanf("%d", &y);
		if((y>=0) && (y<=COLUMNS-1)){
			flag=1;
		}else{
			puts("座標が正しくありません。");
			flag=0;
			continue;
		}

		if(ship->map[y][x]>=0){
			puts("他の戦艦が存在します。\n");
			flag=0;
		}
	}while(flag==0);

	ship->map[y][x]=ship->shipname[n];      //マップ表示用
	ship->battleship[n].position_x=x;       //X軸判定用
	ship->battleship[n].position_y=y;       //Y軸判定用
}

/*所持する戦艦の種類を設定*/
void ShipSelect(Status *ship,int n){	//nは所持戦艦の要素番号
	int sel;
	do{
		printf("%d隻目の種類を選択\n0…%s\n1…%s\n2…%s\n",
				n+1, shipfullname[0], shipfullname[1], shipfullname[2]);
		fflush(0); scanf("%d", &sel);
		ship->shipname[n]=sel;
		if(sel==0){
			//ship->shiplife[n]=SUBMARINE;              //HPへの反映（配列用）
			ship->battleship[n].life=SUBMARINE;       //HPへの反映（判定用）
		}else if(sel==1){
			//ship->shiplife[n]=DESTROYER;
			ship->battleship[n].life=DESTROYER;
		}else if(sel==2){
			//ship->shiplife[n]=WARSHIP;
			ship->battleship[n].life=WARSHIP;
		}else{
			puts("選択が間違っています。");
		}
	}while(sel<0 || sel>2);
	ShipInitialPosition(ship, n);
}

/*所持戦艦のHP表示*/
void CheckShipLife(Status *ship){
	for(int i=0; i<SHIPMAXNUMBER; i++){
		// printf("%d隻目[%s:%c]のHP＝%d\n", i+1, shipfullname[ship->shipname[i]],
		// 		shipmark[i], ship->shiplife[i]);
		printf("%d隻目[%s:%c]のHP＝%d\n", i+1, shipfullname[ship->shipname[i]],
				shipmark[ship->shipname[i]], ship->battleship[i].life);
	}
}

/*指定した(y,x)座標において相手戦艦がいるかどうか判定*/
int CheckHit(Status *ship, int x, int y){
	int px, py;

	if(ship->map[y][x]>=0){		//何かしらの戦艦がいるとき
		for(int i=0; i<SHIPMAXNUMBER; i++){
			if((ship->battleship[i].position_x==x) && (ship->battleship[i].position_y==y)){
				ship->battleship[i].life--;     //Hitした戦艦のHPを下げる
                if(ship->battleship[i].life<=0){
                    ship->map[y][x]=NOSHIP;          //艦隊のHPが0になったらマップに反映
					//複数あるときは破壊された戦艦の非表示や移動できない様に工夫が必要
                }
			}
		}
		return HIT;
	}

	for(py=y-1; py<=y+1; py++){
		if(py<0 || py>=COLUMNS){
			continue;
		}
		for(px=x-1; px<=x+1; px++){
			if(px<0 || px>=ROWS){
				continue;
			}
			if(ship->map[py][px]>=0){		//何かしらの戦艦がいるとき
				return NEAR;
			}
		}
	}
	return MISS;
}

/*プレイヤーの爆撃位置設定*/
void ShootPosition(Status *ship){
	int x, y, hit;
	int flag=0;
	do{
		puts("爆撃する座標の選択");
		printf("x座標:"); fflush(0); scanf("%d", &x);
		printf("y座標:"); fflush(0); scanf("%d", &y);
		if((x>=0) && (x<=ROWS-1)){
			if((y>=0) && (y<=COLUMNS-1)){
				flag=1;
			}else{
				puts("座標が正しくありません。");
			}
		}else{
			puts("座標が正しくありません。");
		}
	}while(flag==0);

	hit=CheckHit(ship, x, y);       //指定した座標での爆撃の開始
    if(hit==HIT){
        puts("着弾しました。");
    }else if(hit==NEAR){
        puts("近くに着弾したようです。");
    }else{
        puts("ミス");
    }
}

/*選択する戦艦の移動*/
void MoveShip(Status *ship){
    int move_ship;
    int move;
    int x, y;       //選択した戦艦の現在位置
    int ox, oy;     //移動オフセット
    int cnt;        //移動番号
    int flag[7];    //移動可能フラグ

    do{
        printf("どの艦隊を移動しますか？\n番号を記入して下さい\n戦艦名 >> 番号\n"); 
        for(int i=0; i<SHIPMAXNUMBER; i++){
            printf("%s >> %d\n",shipfullname[ship->shipname[i]], i);
        }
        fflush(0); scanf("%d", &move_ship);
		if(ship->battleship[move_ship].life<=0){
			puts("その戦艦は破壊されています。");
			continue;
		}
    }while(move_ship<0 || move_ship>=SHIPMAXNUMBER);

    x=ship->battleship[move_ship].position_x;
    y=ship->battleship[move_ship].position_y;
    
    do{
        cnt=1;
        printf("どこに移動しますか？\n");
        for(int py=y-1; py<=y+1; py++){
            for(int px=x-1; px<=x+1; px++){
                if(py==y && px==x){
                    printf("  %c", shipmark[ship->shipname[move_ship]]);
                    cnt--;
                }else if(py<0 || py>=COLUMNS){      //範囲外
                    printf("   ");
                    flag[cnt-1]=0;
                }else if(px<0 || px>=COLUMNS){      //範囲外
                    printf("   ");
                    flag[cnt-1]=0;
                }else if(ship->map[py][px]>=0){     //他の戦艦がいる
                    printf("   ");
                    flag[cnt-1]=0;
                }else{
                    printf("  %d",cnt);
                    flag[cnt-1]=1;
                }
                cnt++;
            }
            putchar('\n');
        }
        fflush(0); scanf("%d", &move);
        putchar('\n');
    }while(flag[move-1]==0);

    if(move>=1 && move<=3){             //オフセットの設定（X軸）
        oy=-1;
    }else if(move==4 || move==5){
        oy=0;
    }else if(move>=6 && move<=8){
        oy=1;
    }

    if(move==1 || move==4 || move==6){  //オフセットの設定（Y軸）
        ox=-1;
    }else if(move==2 || move==7){
        ox=0;
    }else if(move==3 || move==5 || move==8){
        ox=1;
    }

    ship->map[ship->battleship[move_ship].position_y][ship->battleship[move_ship].position_x]=-1;   //戦艦をマップから消去
    ship->battleship[move_ship].position_x+=ox;
    ship->battleship[move_ship].position_y+=oy;
    ship->map[ship->battleship[move_ship].position_y][ship->battleship[move_ship].position_x]=ship->shipname[move_ship];   //戦艦の移動

}

/*戦艦の存在を確認(所持戦艦数のステータスを持たせれば不要)*/
int CheckShip(Status *ship){
	int x, y;
	for(y=0; y<COLUMNS; y++){
		for(x=0; x<ROWS; x++){
			if(ship->map[y][x]>=0){
				return NOTCOMPLETED;
			}
		}
	}
	return COMPLETED;
}

/*COMの艦隊の設定（テスト用）*/
void EnemySetTEST(Status *ship){
    for(int i=0; i<ENEMYSHIPNUMBER; i++){
        ship->battleship[i].life=SUBMARINE;
        ship->battleship[i].position_x=2;
        ship->battleship[i].position_y=2;
        ship->map[2][2]=0;
        ship->shipname[i]=0;    //submarine
    }
}

/*COMの戦艦の設定*/
void EnemySet(Status *ship){
    for(int i=0; i<ENEMYSHIPNUMBER; i++){
    	int ship_kind=rand()%3;				//0 or 1 or 2
    	int x=rand()%ROWS;
    	int y=rand()%COLUMNS;
		ship->battleship[i].life=ship_kind+1;		//SUBMARINE, DESTORYER, WARSHIP
		ship->battleship[i].position_x=x;
		ship->battleship[i].position_y=y;
		ship->map[y][x]=ship_kind;				//Ｓ, Ｄ, Ｗ
		ship->shipname[i]=ship_kind;			//
    }
}

/*プレイヤーターンの行動選択*/
void PlayerTurn(Status *myship, Status *enemyship, int *check_game){
    int turn_end=0;
    int player_total_life=0;
    int enemy_total_life=0;

    puts("プレイヤーの番です。(Press Enter Key)");
	rewind(stdin);	(void)getchar();
    do{
        int act;
        printf("行動を選択(0…攻撃/1…移動/2…HPの表示)");
        fflush(0); scanf("%d", &act);
        if(act==0){
            ShootPosition(enemyship);	    //敵戦艦への砲撃
            turn_end=1;
        }else if(act==1){
            MoveShip(myship);              //移動選択
            turn_end=1;
        }else if(act==2){
            CheckShipLife(myship);		    //味方戦艦の各戦艦のHPチェック
            continue;
        }

        enemy_total_life=0;
        for(int i=0; i<ENEMYSHIPNUMBER; i++){
            enemy_total_life+=enemyship->battleship[i].life;     //敵の全体の体力
        }

        player_total_life=0;
        for(int i=0; i<SHIPMAXNUMBER; i++){
            player_total_life+=myship->battleship[i].life;       //味方の体力
        }

        *check_game=CheckShip(enemyship);
        if(*check_game==COMPLETED){
            puts("全艦破壊しました。\n!!YOU WIN!!\n");
			rewind(stdin);	(void)getchar();
        }else{
            printf("相手の残り体力は%dです。\n", enemy_total_life);	//
			rewind(stdin);	(void)getchar();
        }
    }while(turn_end==0);
}

/*COMターンの行動選択*/
void EnemyTurnEasy(Status *enemyship, Status *myship, int *check_game){
	puts("COMの番です。(Press Enter Key)");
	rewind(stdin);	(void)getchar();

    int action=rand()%2;		//Shoot or Move
    // action=1;			//テスト用 (0:攻撃のみ or 1:移動のみ)
    if(action==0){
    	int x=rand()%ROWS;
    	int y=rand()%COLUMNS;
    	printf("[x=%d][y=%d]に爆撃しました。\n", x, y);
		rewind(stdin);	(void)getchar();
    	if(CheckHit(myship, x, y)==HIT){
    		puts("直撃しました。");
			rewind(stdin);	(void)getchar();
    	}
    }else if(action==1){
		int Bflag=0;
		int action_ship;
		do{
			action_ship=rand()%ENEMYSHIPNUMBER;		//行動する戦艦の選択
			//選択した戦艦の移動可能位置の割り出しと選択が必要
			if(enemyship->battleship[action_ship].life>0){
				Bflag=1;
			}
		}while(Bflag==0);

    	int flag=0;
    	do{
        	int py=rand()%3-1;
        	int px=rand()%3-1;
			int x=enemyship->battleship[action_ship].position_x;
			int y=enemyship->battleship[action_ship].position_y;

			// printf("\npy=%d, px=%d\n", py, px);		//移動ランダムの履歴確認用
        	if(py==0 && px==0)		//同じ位置
        		continue;

        	if(enemyship->map[y+py][x+px]>0)	//他の艦がいる
        		continue;

        	if((y+py)<0 || (y+py)>=COLUMNS)
        		continue;

        	if((x+px)<0 || (x+px)>=ROWS)
        		continue;

        	/*マップ上の入れ替え処理*/
            enemyship->map[y][x]=-1;   //戦艦をマップから消去
            enemyship->battleship[action_ship].position_x+=px;
            enemyship->battleship[action_ship].position_y+=py;
            enemyship->map[enemyship->battleship[action_ship].position_y][enemyship->battleship[action_ship].position_x]=enemyship->shipname[action_ship];   //戦艦の移動
            flag=1;
    	}while(flag==0);
		puts("COMの戦艦が移動しました。");
		rewind(stdin);	(void)getchar();
    }

    *check_game=CheckShip(myship);
    if(*check_game==COMPLETED){
        puts("全艦破壊されました。\n!!YOU LOSE!!");
		rewind(stdin);	(void)getchar();
    }
}

int main(void){
	system("cls");
	srand((unsigned) time(NULL));			//乱数の初期化
	Status myship;		//プレイヤーのステータス
	Status enemyship;	//COMのステータス
    int player_turn=1;  //正負でターンを判断
    int check_game=-1;  //0でゲーム終了

	InitialMAP(&myship);		//プレイヤーステータスの初期化
	InitialMAP(&enemyship);	    //COMステータスの初期化


	for(int i=0; i<SHIPMAXNUMBER; i++){
		ShipSelect(&myship, i);		//プレイヤーの戦艦選択
	}

//    EnemySetTEST(&enemyship);       //COMの戦艦選択(TEST用)
    EnemySet(&enemyship);       //COMの戦艦選択

    while(check_game!=0){
//        PrintMapINT(&myship);		//プレイヤーフィールドの確認(数値)
//        PrintMapINT(&enemyship);		//プレイヤーフィールドの確認(数値)
		system("cls");
		puts("Player Field");
        PrintMapMark(&myship);	        //プレイヤーフィールドの確認(マーク)
		// rewind(stdin);	(void)getchar();

		// puts("Enemy Field");
        // PrintMapMark(&enemyship);	    //相手のフィールドの確認用(マーク)
		// rewind(stdin);	(void)getchar();

		//プレイヤーターンの交代
        if(player_turn>0){
            PlayerTurn(&myship, &enemyship, &check_game);
        }else{
            EnemyTurnEasy(&enemyship, &myship, &check_game);
        }
        player_turn *= -1;
    }
	return 0;
}
