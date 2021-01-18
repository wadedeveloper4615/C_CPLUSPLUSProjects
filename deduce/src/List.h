//**********************************************************
//** Deduce copyright (c) 2020 Christopher D. Wade        **
//** File: list.h                                         **
//**********************************************************
#ifndef LIST_H_
#define LIST_H_

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <locale.h>
#include <tchar.h>
#include <shellapi.h>

template<class T>
class Node {
        T data;
    public:
        Node<T> *next;
    public:
        Node<T>(const T &d) : data(d), next(NULL) {
        }
        Node<T>(const T &d, Node<T> *next) : data(d), next(next) {
        }
        Node<T>(const Node<T> &node) : data(node.data), next(NULL) {
        }
};

template<class T>
class LinkedList {
        Node<T> *list;
        int numberOfElements;
    public:
        LinkedList() : list(NULL), numberOfElements(0) {
        }
        ~LinkedList() {
        }
        void addAtStart(const T &data) {
            Node<T> *ptr = list;
            list = new Node<T>(data, ptr);
            numberOfElements++;
        }
        void addAtEnd(const T &data) {
            if (list != NULL) {
                Node<T> *ptr = list;
                while (ptr->next != NULL) {
                    ptr = ptr->next;
                }
                ptr->next = new Node<T>(data);
            } else {
                list = new Node<T>(data);
            }
            numberOfElements++;
        }
};

#endif /* LIST_H_ */
