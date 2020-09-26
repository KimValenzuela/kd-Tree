#include "kdTreeADT.h"
#include "kdTree.h"

using namespace std;


kdTree::kdTree(int n, int m){
	root = NULL;
	tree = NULL;

	if(n >= m) D = n;
	else D = m;

	x_1 = 0;
	y_1 = 0;
	x_2 = D;
	y_2 = D;
}

//-------------------------------------------------------
//BUILD

void kdTree::build(node*& tree, vector<pair<int,int>> vecX, vector<pair<int,int>> vecY, bool nivel){
	if(vecX.size() == 1){
		tree = newNode(vecX[0]);
		return;
	}
	else{
		if(nivel){
			int midd = vecX.size()/2;
			if(vecX.size()%2 != 0) midd += 1;
			vector<pair<int,int>> xl, xr, yl, yr;
			for(int i=0 ; i<midd ; i++) xl.push_back(vecX[i]);
			for(int i=midd ; i<vecX.size() ; i++) xr.push_back(vecX[i]);
			for(int i=0 ; i<vecY.size() ; i++){
				if(vecY[i].first < vecX[midd].first) yl.push_back(vecY[i]);
				else if(vecY[i].first >= vecX[midd].first) yr.push_back(vecY[i]);
			}
			if(root == NULL){
				root = newNode(vecX[midd]);
				tree = root;
				build(tree->left, xl, yl, !nivel);
				build(tree->right, xr, yr, !nivel);
			}else{
				tree = newNode(vecX[midd]);
				build(tree->left, xl, yl, !nivel);
				build(tree->right, xr, yr, !nivel);
			}
		}else{
			int midd = vecY.size()/2;
			if(vecX.size()%2 != 0) midd += 1;
			vector<pair<int,int>> xl, xr, yl, yr;
			for(int i=0 ; i<midd ; i++) yl.push_back(vecY[i]);
			for(int i=midd ; i<vecY.size() ; i++) yr.push_back(vecY[i]);
			for(int i=0 ; i<vecX.size() ; i++){
				if(vecX[i].second < vecY[midd].second) xl.push_back(vecX[i]);
				else if(vecX[i].second >= vecY[midd].second) xr.push_back(vecX[i]); 
			}
			if(root == NULL){
				root = newNode(vecY[midd]);
				tree = root;
				build(tree->left, xl, yl, !nivel);
				build(tree->right, xr, yr, !nivel);
			}else{
				tree = newNode(vecY[midd]);
				build(tree->left, xl, yl, !nivel);
				build(tree->right, xr, yr, !nivel);
			}
		}
	}
}

kdTree::node* kdTree::newNode(pair<int,int> p){
	struct node* n = new node();
	n->point = p;
	n->left = NULL;
	n->right = NULL;
	return n;
}

void kdTree::insert(pair<int,int> p){
	x_sort.push_back(p);
	y_sort.push_back(p);
}

bool kdTree::sortCoordY(const pair<int,int> &a, const pair<int,int> &b){
	return (a.second < b.second);
}

void kdTree::sortPoints(){
	//Ordenar vector por coordenada X
	sort(x_sort.begin(),x_sort.end());
	//Ordenar vector por coordenada Y
	sort(y_sort.begin(),y_sort.end(), sortCoordY);

	build(tree, x_sort, y_sort, true);
}

//------------------------------------------------------------------
//SEARCH

void kdTree::search(pair<int,int> p1, pair<int,int> p2){
	b1 = p1;
	b2 = p2;
	rangeSearch(root, true, x_1, y_1, x_2, y_2);
	if(pSearch.size() == 0) cout<<"No se han encontrado puntos en esta region."<<endl;
	else{
		for(int i=0 ; i <pSearch.size() ; i++){
			cout<<pSearch[i].first<<","<<pSearch[i].second<<endl;
		}
	}
}

void kdTree::rangeSearch(node* r, bool nivel, int x1, int y1, int x2, int y2){
	if(isLeaf(r)){
		if(r->point.first >= b1.first && r->point.first <= b2.first && 
			r->point.second >= b1.second && r->point.second <= b2.second){
			pSearch.push_back(r->point);
		}else return;
	}else{
		if(nivel){
			if(b1.first <= x1 && b2.first >= r->point.first && b1.second <= y1 && b2.second >= y2){
				preorden(r->left);
			}else if(intersecta(x1, y1, r->point.first, y2, b1.first, b1.second, b2.first, b2.second)){	
				rangeSearch(r->left, !nivel, x1, y1, r->point.first - 1, y2);
			}

			if(b1.first <= r->point.first && b2.first >= x2 && b1.second <= y1 && b2.second >= y2){
				preorden(r->right);
			}else if(intersecta(r->point.first, y1, x2, y2, b1.first, b1.second, b2.first, b2.second)){
				rangeSearch(r->right, !nivel, r->point.first, y1, x2, y2);
			}
		}else{
			if(b1.first <= x1 && b2.first >= x2 && b1.second <= y1 && b2.second >= r->point.second){
				preorden(r->left);
			}else if(intersecta(x1, y1, x2, r->point.second, b1.first, b1.second, b2.first, b2.second)){
				rangeSearch(r->left, !nivel, x1, y1, x2, r->point.second - 1);
			}

			if(b1.first <= x1 && b2.first >= x2 && b1.second <= r->point.second && b2.second >= y2){
				preorden(r->right);
			}else if(intersecta(x1, r->point.second, x2, y2, b1.first, b1.second, b2.first, b2.second)){
				rangeSearch(r->right, !nivel, x1, r->point.second, x2, y2);
			}
		}
	}

}

bool kdTree::intersecta(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
	//rect 1 = (x1,y1) & (x2,y2)
	//rect 2 = (x3,y3) & (x4,y4)
	//max{(x1,y1),(x3,y3)} & min{(x2,y2),(x4,y4)}
	int ix1, iy1, ix2, iy2; //Las 4 coords. de la interseccion

	ix1 = max(x1,x3); 
	iy1 = max(y1,y3); 
	ix2 = min(x2,x4); 
	iy2 = min(y2,y4); 

	if ((ix1 > ix2) || (iy1 > iy2)) return false;
	else return true;

}

void kdTree::preorden(node *n){
	if(n == NULL) return;
	else{
		if(isLeaf(n)){
			pSearch.push_back(n->point);
		}else{
			preorden(n->left);
			preorden(n->right);
		}
	}
}

bool kdTree::isLeaf(node* n){
	if(n->right == NULL && n->left == NULL) return true;
	else return false;
}