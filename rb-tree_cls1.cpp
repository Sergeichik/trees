
template<typename T>
class ITree {
public:
	virtual void insert(T) = 0;
	virtual void erase(T) = 0;
};


//! класс дерева
template <typename T>
class RBtree: public ITree<T>{
public:
	struct node_st { 
		node_st *p1;
		node_st *p2;
		T value; 
		bool red; 
	}; // структура узла
	node_st *tree_root;					//!< корень
	int nodes_count;					//!< число узлов дерева
	void Show();						//!< вывод дерева
	RBtree();
	~RBtree();
	void Clear();			//!< снести дерево				
	node_st* find(T);			//!< найти значение
	void insert(T);		//!< вставить значение
	void erase(T);		//!< удалить значение
	int GetNodesCount();	//!< узнать число узлов
private:
	node_st *NewNode(T value);		//!< выделение новой вешины
	void DelNode(node_st*);				//!< удаление вершины
	void Clear(node_st*);				//!< снос дерева (рекурсивная часть)
	node_st *Rotate21(node_st*);		//!< вращение влево
	node_st *Rotate12(node_st*);		//!< вращение вправо
	void BalanceInsert(node_st**);		//!< балансировка вставки
	bool BalanceRemove1(node_st**);		//!< левая балансировка удаления
	bool BalanceRemove2(node_st**);		//!< правая балансировка удаления
	bool insert(T, node_st**);			//!< рекурсивная часть вставки
	bool GetMin(node_st**, node_st**);	//!< найти и убрать максимальный узел поддерева
	bool erase(node_st**, T);			//!< рекурсивная часть удаления
	void Show(node_st*, int, char);		//!< вывод дерева, рекурсивная часть

};



template <typename T>
RBtree<T>::RBtree()
{
	tree_root = 0;
	nodes_count = 0;
}

template <typename T>
RBtree<T>::~RBtree()
{
	Clear(tree_root);
}

template <typename T>
int RBtree<T>::GetNodesCount()
{
	return nodes_count;
}

// выделение новой вешины
template <typename T>
typename RBtree<T>::node_st *RBtree<T>::NewNode(T value)
{
	nodes_count++;
	node_st *node = new node_st;
	node->value = value;
	node->p1 = node->p2 = 0;
	node->red = true;
	return node;
}

// удаление вершины
template <typename T>
void RBtree<T>::DelNode(node_st *node)
{
	nodes_count--;
	delete node;
}

// снос дерева (рекурсивная часть)
template <typename T>
void RBtree<T>::Clear(node_st *node)
{
	if (!node) return;
	Clear(node->p1);
	Clear(node->p2);
	DelNode(node);
}

// вывод дерева, рекурсивная часть
//! \param node узел
//! \param depth глубина
//! \param dir   значёк
//! \code Show(root,0,'*'); \endcode
template <typename T>
void RBtree<T>::Show(node_st *node, int depth, char dir)
{
	int n;
	if (!node) return;
	for (n = 0; n < depth; n++) putchar(' ');
	printf("%c[%d] (%s)\n", dir, node->value, node->red ? "red" : "black");
	Show(node->p1, depth + 2, '-');
	Show(node->p2, depth + 2, '+');
}


// вращение влево
//! \param index индеск вершины
//! \result новая вершина дерева
template <typename T>
typename RBtree<T>::node_st *RBtree<T>::Rotate21(node_st *node)
{
	node_st *p2 = node->p2;
	node_st *p21 = p2->p1;
	p2->p1 = node;
	node->p2 = p21;
	return p2;
}

// вращение вправо
//! \param index индеск вершины
//! \result новая вершина дерева
template <typename T>
typename RBtree<T>::node_st *RBtree<T>::Rotate12(node_st *node)
{
	node_st *p1 = node->p1;
	node_st *p12 = p1->p2;
	p1->p2 = node;
	node->p1 = p12;
	return p1;
}


// балансировка вершины
template <typename T>
void RBtree<T>::BalanceInsert(node_st **root)
{
	node_st *p1, *p2, *px1, *px2;
	node_st *node = *root;
	if (node->red) return;
	p1 = node->p1;
	p2 = node->p2;
	if (p1 && p1->red) {
		px2 = p1->p2;				// задача найти две рядом стоящие крастне вершины
		if (px2 && px2->red) p1 = node->p1 = Rotate21(p1);
		px1 = p1->p1;
		if (px1 && px1->red) {
			node->red = true;
			p1->red = false;
			if (p2 && p2->red) {	// отделаемся перекраской вершин
				px1->red = true;
				p2->red = false;
				return;
			}
			*root = Rotate12(node);
			return;
		}
	}
	// то же самое в другую сторону
	if (p2 && p2->red) {
		px1 = p2->p1;				// задача найти две рядом стоящие крастне вершины
		if (px1 && px1->red) p2 = node->p2 = Rotate12(p2);
		px2 = p2->p2;
		if (px2 && px2->red) {
			node->red = true;
			p2->red = false;
			if (p1 && p1->red) {	// отделаемся перекраской вершин
				px2->red = true;
				p1->red = false;
				return;
			}
			*root = Rotate21(node);
			return;
		}
	}
}

template <typename T>
bool RBtree<T>::BalanceRemove1(node_st **root)
{
	node_st *node = *root;
	node_st *p1 = node->p1;
	node_st *p2 = node->p2;
	if (p1 && p1->red) {
		p1->red = false; return false;
	}
	if (p2 && p2->red) { // случай 1
		node->red = true;
		p2->red = false;
		node = *root = Rotate21(node);
		if (BalanceRemove1(&node->p1)) node->p1->red = false;
		return false;
	}
	unsigned int mask = 0;
	node_st *p21 = p2->p1;
	node_st *p22 = p2->p2;
	if (p21 && p21->red) mask |= 1;
	if (p22 && p22->red) mask |= 2;
	switch (mask)
	{
	case 0:		// случай 2 - if((!p21 || !p21->red) && (!p22 || !p22->red))
		p2->red = true;
		return true;
	case 1:
	case 3:		// случай 3 - if(p21 && p21->red)
		p2->red = true;
		p21->red = false;
		p2 = node->p2 = Rotate12(p2);
		p22 = p2->p2;
	case 2:		// случай 4 - if(p22 && p22->red)
		p2->red = node->red;
		p22->red = node->red = false;
		*root = Rotate21(node);
	}
	return false;
}

template <typename T>
bool RBtree<T>::BalanceRemove2(node_st **root)
{
	node_st *node = *root;
	node_st *p1 = node->p1;
	node_st *p2 = node->p2;
	if (p2 && p2->red) { p2->red = false; return false; }
	if (p1 && p1->red) { // случай 1
		node->red = true;
		p1->red = false;
		node = *root = Rotate12(node);
		if (BalanceRemove2(&node->p2)) node->p2->red = false;
		return false;
	}
	unsigned int mask = 0;
	node_st *p11 = p1->p1;
	node_st *p12 = p1->p2;
	if (p11 && p11->red) mask |= 1;
	if (p12 && p12->red) mask |= 2;
	switch (mask) {
	case 0:		// случай 2 - if((!p12 || !p12->red) && (!p11 || !p11->red))
		p1->red = true;
		return true;
	case 2:
	case 3:		// случай 3 - if(p12 && p12->red)
		p1->red = true;
		p12->red = false;
		p1 = node->p1 = Rotate21(p1);
		p11 = p1->p1;
	case 1:		// случай 4 - if(p11 && p11->red)
		p1->red = node->red;
		p11->red = node->red = false;
		*root = Rotate12(node);
	}
	return false;
}

template <typename T>
typename RBtree<T>::node_st* RBtree<T>::find(T value)
{
	node_st *node = tree_root;
	while (node) {
		if (node->value == value) return node;
		node = node->value > value ? node->p1 : node->p2;
	}
	return 0;
}


// рекурсивная часть вставки
//! \result true если изменений не было или балансировка в данной вершине не нужна
template <typename T>
bool RBtree<T>::insert(T value, node_st **root)
{
	node_st *node = *root;
	if (!node) *root = NewNode(value);
	else {
		if (value == node->value) return true;
		if (insert(value, value < node->value ? &node->p1 : &node->p2)) return true;
		BalanceInsert(root);
	}
	return false;
}


// найти и убрать максимальный узел поддерева
//! \param root корень дерева в котором надо найти элемент
//! \retval res эелемент который был удалён
//! \result true если нужен баланс
template <typename T>
bool RBtree<T>::GetMin(node_st **root, node_st **res)
{
	node_st *node = *root;
	if (node->p1) {
		if (GetMin(&node->p1, res)) return BalanceRemove1(root);
	}
	else {
		*root = node->p2;
		*res = node;
		return !node->red;
	}
	return false;
}


// рекурсивная часть удаления
//! \result true если нужен баланс
template <typename T>
bool RBtree<T>::erase(node_st **root, T value)
{
	node_st *t, *node = *root;
	if (!node) return false;
	if (node->value < value) {
		if (erase(&node->p2, value))	return BalanceRemove2(root);
	}
	else if (node->value > value) {
		if (erase(&node->p1, value))	return BalanceRemove1(root);
	}
	else {
		bool res;
		if (!node->p2) {
			*root = node->p1;
			res = !node->red;
		}
		else {
			res = GetMin(&node->p2, root);
			t = *root;
			t->red = node->red;
			t->p1 = node->p1;
			t->p2 = node->p2;
			if (res) res = BalanceRemove2(root);
		}
		DelNode(node);
		return res;
	}
	return 0;
}


// вывод дерева
template <typename T>
void RBtree<T>::Show()
{
	printf("[tree]\n");
	Show(tree_root, 0, '*');
}

// функция вставки
template <typename T>
void RBtree<T>::insert(T value)
{
	insert(value, &tree_root);
	if (tree_root) tree_root->red = false;
}

// удаление узла
template <typename T>
void RBtree<T>::erase(T value)
{
	erase(&tree_root, value);
}

// снос дерева
template <typename T>
void RBtree<T>::Clear()
{
	Clear(tree_root);
	tree_root = 0;
}
