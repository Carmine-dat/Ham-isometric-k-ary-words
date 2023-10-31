#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <divsufsort.h>
#include <sdsl/rmq_support.hpp>
#include <list>

using namespace std;
using namespace sdsl;

#define maxNum(a, b) a > b ? a : b
#define minNum(a, b) a < b ? a : b

bool checkWord(char*,int);
char *strPrefix(char*, int);
int twoErrorOverlaps(char*,int*,int*,int**,int*,int*,rmq_succinct_sct<>);
int LCParray(char*,int,int*,int*,int*);
int HammingDistance(char,char);
int LCA(int,int,int*,int*,rmq_succinct_sct<>);
bool condPlus(char*,int,int,int,int*,int*,rmq_succinct_sct<>);
int stringBuilder(char*,int,string*,char*);
int witnessesConstructor(char*,int,int*,string*,string*,int*,int*,
	rmq_succinct_sct<>);

int main(){
	
	int *SA;
	int *ISA;
	int *LCP;
	char word[128];
	bool flag;
  int k;

  do{
    flag = false;
    printf("Insert the size k of the alphabet (k >= 2): ");
    scanf("%d", &k);
    if(k < 2){
      flag = true;
      printf("Size not valid.\n");
    }
  }while(flag);

  do{
    flag = true;
    if(k <= 10)
    	printf("Insert a word on the alphabet {0..%d}): ", k - 1);
    else
    	printf("Insert a word on the alphabet {0..9, A..%c}): ", 65 - 10 + k - 1);
    scanf("%s", word);
    flag = checkWord((char*) word, k);
    if(!flag) printf("Word not valid.\n");
  }while(!flag);

	int n = strlen(word);

	SA = (int*) calloc(n, sizeof(int));

	if(SA == NULL){
		fprintf(stderr, "Error: Cannot allocate memory for SA.\n");
	  exit(EXIT_FAILURE);
	}

	if(divsufsort((unsigned char*) word, SA, n) != 0){
	  fprintf(stderr, "Error: SA computation failed.\n");
	  exit(EXIT_FAILURE);
	}

	//print suffix array
	printf("\n");
	for(int i = 0; i < n; i++){
		printf("SA[%2d]: %2d -> ", i, SA[i]);
		for(int j = SA[i]; j < n; j++){
			printf("%c", word[j]);
		}
		printf("\n");
	}
	printf("\n");

	ISA = (int*) calloc(n, sizeof(int));

	if(ISA == NULL){
		fprintf(stderr, "Error: Cannot allocate memory for ISA.\n");
	  exit(EXIT_FAILURE);
	}

	for(int i = 0; i < n; i++)
		ISA[SA[i]] = i;

	//print inverse suffix array
	for(int i = 0; i < n; i++)
		printf("ISA[%2d]: %2d\n", i, ISA[i]);
	printf("\n");

	LCP = (int*) calloc(n, sizeof(int));

	if(LCP == NULL){
		fprintf(stderr, "Error: Cannot allocate memory for LCP.\n");
	  exit(EXIT_FAILURE);
	}

	if(LCParray((char*) word, n, SA, ISA, LCP) != 0){
	  fprintf(stderr, "Error: LCP computation failed.\n");
	  exit(EXIT_FAILURE);
	}

	//print lcp array
	for(int i = 0; i < n; i++)
		printf("LCP[%2d]: %2d\n", i, LCP[i]);
	printf("\n");

	int_vector<> vect(n, 0);

  for(int i = 0; i < n; i ++)
    vect[i] = LCP[i];

  rmq_succinct_sct<> rmq(&vect);
 	util::clear(vect);

	int *twoeolens, **allerrpos, nTwoErrorOverlaps, I;

	twoeolens = (int*) calloc(n, sizeof(int));

	if(twoeolens == NULL){
		fprintf(stderr, "Error: Cannot allocate memory for twoeolens.\n");
		exit(EXIT_FAILURE);
	}

	allerrpos = (int**) calloc(n, sizeof(int*));

	if(allerrpos == NULL){
		fprintf(stderr, "Error: Cannot allocate memory for allerrpos.\n");
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < n; i++){
		allerrpos[i] = (int*) calloc(2, sizeof(int));
		if(allerrpos[i] == NULL){
			fprintf(stderr, "Error: Cannot allocate memory for allerrpos.\n");
		  exit(EXIT_FAILURE);
		}
	}

	if(twoErrorOverlaps(word,twoeolens,&nTwoErrorOverlaps,allerrpos,LCP,ISA,rmq) != 0){
		fprintf(stderr, "Error occurred during 2-error-overlaps search.\n");
		exit(EXIT_FAILURE);
	}

	printf("\nnTwoErrorOverlaps: %d\n", nTwoErrorOverlaps);

	string u(2*n, '0');
	string v(2*n, '0');
	string utmp(2*n, '0');
	string vtmp(2*n, '0');

	if(nTwoErrorOverlaps != 0){
		I = 2*n;
		for(int i = 0; i < nTwoErrorOverlaps; i++){
			utmp.clear();
			vtmp.clear();
	    witnessesConstructor(word,twoeolens[i],allerrpos[i],&utmp,&vtmp,LCP,ISA,rmq);
	    if(utmp.length() < I){
        I = utmp.length();
        u = utmp;;
        v = vtmp;
      }
		}

		cout << "\nThe word is non-Ham-isometric" << endl;
		cout << "Index: " << I << endl << endl;
		cout << "Witnesses:" << endl << endl;
		cout << "u: " << u << endl;
		cout << "v: " << v << endl;
	}
	else printf("\nThe word is Ham-isometric\n");

	return 0;
}

bool checkWord(char *word, int k){

  bool flag = true;

  if(k <= 10)
    for(int i = 0; i < strlen(word); i++)
      if(word[i] < 48 || word[i] > 48 + k - 1){
        flag = false;
        break;
      }
  else
    for(int i = 0; i < strlen(word); i++)
      if((word[i] < 48 || word[i] > 57) && (word[i] < 65 || word[i] > 65 - 10 + k - 1)){
        flag = false;
        break;
      }

  return flag;
}

int twoErrorOverlaps(char *f, int *twoeolens, int *nTwoErrorOverlaps,
	int **allerrpos, int *LCP, int *ISA, rmq_succinct_sct<> rmq){

  int n = strlen(f);
	int *allerrpostmp = (int*) calloc(2, sizeof(int));

	if(allerrpostmp == NULL){
    fprintf(stderr, "Error: Cannot allocate memory for allerpostmp.\n");
    exit(EXIT_FAILURE);
  }

  *nTwoErrorOverlaps = 0;

  int *nAllerrpos = 0;
  int nAllerrpostmp = 0;
  int l = 0, d = 0;

  for (int i = 1; i <= n - 2; i++){
   	l = 0;
    d = 0;

    for(int i = 0; i < n; i++)
    	*allerrpostmp = 0;

    nAllerrpostmp = 0;

    while (d <= 2){

      if (l < n - i){
        int lca = LCA(l, i+l, ISA, LCP, rmq);
        l = l + lca;
      }

      if (l < n - i){
        allerrpostmp[nAllerrpostmp] = l;
        nAllerrpostmp++;
      }

      if (d == 2 && l == n - i){
        twoeolens[*nTwoErrorOverlaps] = l;
        std::copy(allerrpostmp,allerrpostmp+nAllerrpostmp,allerrpos[*nTwoErrorOverlaps]);
        *nTwoErrorOverlaps = *nTwoErrorOverlaps + 1;
      }

      if(d < 2 && l < n - i){
        d = d + HammingDistance(f[l], f[i+l]);
        l++;
      }
      else break;
    }
  }

  printf("twoeolens: [ ");
  for(int i = 0; i < n; i++)
  	printf("%d ", twoeolens[i]);
  printf("]\n");

  printf("\nallerpos:\n");
  for(int i = 0; i < n; i++){
		printf("| ");
		for(int j = 0; j < 2; j++)
			printf("%d ", allerrpos[i][j]);
		printf("|\n");
  }

  return 0;
}

int witnessesConstructor(char *f, int l, int *errpos, string *u, string *v, int *LCP,
	int *ISA, rmq_succinct_sct<> rmq){
    
  int n = strlen(f);
  int r = n - l;
  int i = errpos[0];
  int j = errpos[1];
  bool cplus = condPlus(f, r, errpos[0], errpos[1], LCP, ISA, rmq);

  if (cplus == false){
		char *falfa = (char*) calloc(n, sizeof(char));
	  char *fbeta = (char*) calloc(n, sizeof(char));
	  strcpy(falfa, f);
	  strcpy(fbeta, f);

	  falfa[i] = f[r + i];
	  fbeta[j] = f[r + j];

	  if(stringBuilder(f, r, u, falfa) != 0){
	    fprintf(stderr, "Error: building the word for witnessConstructor.\n");
	    exit(EXIT_FAILURE);
	  }

	  if(stringBuilder(f, r, v, fbeta) != 0){
	    fprintf(stderr, "Error: building the word for witnessConstructor.\n");
	    exit(EXIT_FAILURE);
	  }
  }
  else
		if(i <= r/2){
      char *feta = (char*) calloc(n, sizeof(char));
      char *fgamma = (char*) calloc(n, sizeof(char));
      strcpy(feta, f);
      strcpy(fgamma, f);

      feta[i] = f[r + i];
      fgamma[j] = f[r + j];
      fgamma[r/2 + j] = f[i];

      if(stringBuilder(f, r, u, feta) != 0){
        fprintf(stderr, "Error: building the word for witnessConstructor.\n");
        exit(EXIT_FAILURE);
      }

      if(stringBuilder(f, r, v, fgamma) != 0){
        fprintf(stderr, "Error: building the word for witnessConstructor.\n");
        exit(EXIT_FAILURE);
      }

      string suff(f + n - r/2);
      *u = *u + suff;
      *v = *v + suff;
		}

  return 0;
}

bool condPlus(char *f, int r, int i, int j, int *LCP, int *ISA,
	rmq_succinct_sct<> rmq){
    
	bool cond1 = false, cond2 = false, cond3 = false;
	int n = strlen(f);

	if(r % 2 == 0){
		if(j - i == r/2){
			cond1 = true;
			if(f[r + i] == f[r + j]){
				cond2 = true;
				if(j < n){
					if(LCA(i, j, ISA, LCP, rmq) >= r/2){
						cond3 = true;
					}
					else{
						if(r/2 == 0){
							cond3 = true;
						}
					}
				}
			}
		}
	}

  return cond1 && cond2 && cond3;
}

char *strPrefix(char *str, int r){

	char *prefix = (char*) calloc(r + 1, sizeof(char));

	if(prefix == NULL){
	  fprintf(stderr, "Error: Cannot allocate memory for prefix.\n");
	  exit(EXIT_FAILURE);
	}

	for(int i = 0; i < r; i++)
	  prefix[i] = str[i];

	prefix[r]= '\0';

	return prefix;
}

int LCParray(char *text, int n, int *SA, int *ISA, int *LCP){                           
	
	int i = 0, j = 0;
	LCP[0] = 0;

	for(i = 0; i < n; i++)
	  if(ISA[i] != 0){
	  	if(i == 0) j = 0;
	  	else j = (LCP[ISA[i - 1]] >= 2) ? LCP[ISA[i - 1]] - 1 : 0;
	  	while(text[i + j] == text[SA[ISA[i] - 1] + j])
	  		j++;
	  	LCP[ISA[i]] = j;
	  }

	return 0;
}

int HammingDistance(char a, char b){
	return a == b ? 0 : 1;
}

int LCA(int l, int r, int *ISA, int *LCP, rmq_succinct_sct<> rmq){
    int lmin = minNum(ISA[l], ISA[r]);
    int rmax = maxNum(ISA[l], ISA[r]);
    return LCP[rmq(lmin + 1, rmax)];
}

int stringBuilder(char *f, int prefixLenght, string *s, char *addOn){
	*s = strPrefix(f, prefixLenght);
	string suff(addOn);
	*s = *s + suff;
	return 0;
}