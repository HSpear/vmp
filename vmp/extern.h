

#ifndef   __MY_EXTERN_H
#define   __MY_EXTERN_H

/************************************************************************/
/* public api                                                           */
/************************************************************************/

template <class T>
void _FreeVector( vector<T>& vc)
{
	vc.clear();
}

template <class T>
void _FreeSet( set<T>& s)
{
	s.clear();
}

extern void _Conver_Code_To_String( string &s, const vector<pnode>& nd);
extern void _Conver_Code_To_String( string &s, const vector<_struct_data>& vc);


#endif    __MY_EXTERN_H
