#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
	WP *f, *p;
	f = free_;
	if (f == NULL) assert(0);
	free_ = free_->next;
	f->next = NULL;
	p = head;
	if (p == NULL) {
		head = f;
		p = head;
	}
	else {
		while (p->next != NULL) p = p->next;
		p->next = f;
	}	
	return f;
}

void free_wp(WP *wp) {
	WP *f, *p;
	p = free_;
	if (p == NULL) {
		free_ = wp;
		p = free_;
	}
	else {
		while (p->next != NULL) p = p->next;
		p->next = wp;
	}
	f = head;
	if (head == NULL) assert(0);
	if (head->NO == wp->NO) {
		head = head->next;
	}
	else {
		while (f->next != NULL && f->next->NO != wp->NO)
			f = f->next;
		if (f->next->NO == wp->NO)
			f->next = f->next->next;
		else assert(0);
	}
	wp->next = NULL;
	wp->val = 0;
	wp->str[0] = '\0';
}

bool check_wp() {
	WP *f;
	f = head;
	bool key = true;
	bool suc;
	while (f != NULL) {
		uint32_t tmp_expr = expr(f->str, &suc);
		if (!suc) assert(0);
		if (tmp_expr != f->val) {
			key = false;
			printf("Watchpoint %d: %s\n", f->NO, f->str);
			printf("Old value = %d\n", f->val);
			printf("New value = %d\n", tmp_expr);
			f->val = tmp_expr;
		}
		f = f->next;
	}
	return key;
}

void delete_wp(int num) {
	WP *f;
	f = &wp_pool[num];
	free_wp(f);
}

void info_wp() {
	WP *f;
	f = head;
	while (f != NULL) {
		printf("Watchpoint %d: %s = %d\n", f->NO, f->str, f->val);
		f = f->next;
	}
}
