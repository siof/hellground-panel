// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef WT_DBO_DBO_PTR_IMPL_H_
#define WT_DBO_DBO_PTR_IMPL_H_

#include <stdexcept>

namespace Wt {
  namespace Dbo {
    namespace Impl {

      template <class C, typename T>
      void setAutogeneratedId(MetaDbo<C>& dbo, const T& currentId,
			      long long id) { }

      template <class C>
      void setAutogeneratedId(MetaDbo<C>& dbo, const long long& currentId,
			      long long id) {
	dbo.setId(id);
      }
    }

template <class C>
MetaDbo<C>::~MetaDbo()
{
  if (refCount_)
    throw std::logic_error("Dbo: refCount > 0");

  if ((!isOrphaned()) && session())
    session()->prune(this);

  delete obj_;
}

template <class C>
void MetaDbo<C>::flush()
{
  checkNotOrphaned();
  if (state_ & NeedsDelete) {
    state_ &= ~NeedsDelete;

    try {
      session()->implDelete(*this);
      setTransactionState(DeletedInTransaction);
    } catch (...) {
      setTransactionState(DeletedInTransaction);
      throw;
    }

  } else if (state_ & NeedsSave) {
    state_ &= ~NeedsSave;

    try {
      session()->implSave(*this);
      setTransactionState(SavedInTransaction);
    } catch (...) {
      setTransactionState(SavedInTransaction);
      throw;
    }
  }
}

template <class C>
void MetaDbo<C>::bindId(SqlStatement *statement, int& column)
{
  Session::MappingInfo *mapping = session()->template getMapping<C>();

  SaveBaseAction action(*this, *mapping, statement, column);

  field(action, id_,
	mapping->naturalIdFieldName, mapping->naturalIdFieldSize);

  column = action.column();
}

template <class C>
void MetaDbo<C>::bindId(std::vector<Impl::ParameterBase *>& parameters)
{
  parameters.push_back(new Impl::Parameter<typename dbo_traits<C>::IdType>(id_));
}

template <class C>
void MetaDbo<C>::prune()
{
  checkNotOrphaned();
  session()->prune(this);
  setId(dbo_traits<C>::invalidId());
  setVersion(-1);
  setState(New);
}

template <class C>
void MetaDbo<C>::transactionDone(bool success)
{
  Session *s = session();

  if (success) {
    if (deletedInTransaction()) {
      prune();
      setSession(0);
    } else if (savedInTransaction()) {
      setVersion(version() + 1);
      setState(Persisted);
    } 
  } else {
    if (deletedInTransaction()) {
      state_ |= NeedsDelete;
      session()->needsFlush(this);
    } else if (savedInTransaction()) {
      if (isNew()) {
	prune();
      } else {
	/*
	 * If we support changing the Id, then we need to restore the old
	 * Id here.
	 */
	state_ |= NeedsSave;
	session()->needsFlush(this);
      }
    }
  }

  if (obj_)
    s->implTransactionDone(*this, success);

  resetTransactionState();
}

template <class C>
void MetaDbo<C>::purge()
{
  checkNotOrphaned();
  if (isPersisted() && !isDirty() && !inTransaction()) {
    delete obj_;
    obj_ = 0;
    setVersion(-1);
  }
}

template <class C>
void MetaDbo<C>::reread()
{
  checkNotOrphaned();
  if (isPersisted()) {
    session()->discardChanges(this);

    delete obj_;
    obj_ = 0;
    setVersion(-1);

    state_ = Persisted;
  }
}

template <class C>
void MetaDbo<C>::setObj(C *obj)
{
  checkNotOrphaned();
  obj_ = obj;
  DboHelper<C>::setMeta(*obj, this);
}

template <class C>
C *MetaDbo<C>::obj()
{
  checkNotOrphaned();
  if (!obj_ && !isDeleted())
    doLoad();

  return obj_;
}

template <class C>
MetaDbo<C>::MetaDbo(C *obj)
  : MetaDboBase(-1, New | NeedsSave, 0),
    obj_(obj),
    id_(dbo_traits<C>::invalidId())
{ 
  DboHelper<C>::setMeta(*obj_, this);
}

template <class C>
MetaDbo<C>::MetaDbo(const IdType& id, int version, int state,
		    Session& session, C *obj)
  : MetaDboBase(version, state, &session),
    obj_(obj),
    id_(id)
{
  if (obj_)
    DboHelper<C>::setMeta(*obj_, this);
}

template <class C>
void MetaDbo<C>::setAutogeneratedId(long long id)
{
  Impl::setAutogeneratedId(*this, id_, id);
}

template <class C>
void MetaDbo<C>::doLoad()
{
  int column = 0;
  session()->template implLoad<C>(*this, 0, column);
  DboHelper<C>::setMeta(*obj_, this);
}

template <class C>
ptr<C>::ptr(C *obj)
  : obj_(0)
{
  if (obj) {
    obj_ = new MetaDbo<C>(obj);
    takeObj();
  }
}

template <class C>
ptr<C>::ptr(const ptr<C>& other)
  : obj_(other.obj_)
{
  takeObj();
}

template <class C>
ptr<C>::~ptr()
{
  freeObj();
}

template <class C>
void ptr<C>::reset(C *obj)
{
  freeObj();
  if (obj) {
    obj_ = new MetaDbo<C>(obj);
    takeObj();
  }
}

template <class C>
ptr<C>& ptr<C>::operator= (const ptr<C>& other)
{
  if (obj_ != other.obj_) {
    freeObj();
    obj_ = other.obj_;
    takeObj();
  }

  return *this;
}

template <class C>
const C *ptr<C>::operator->() const
{
  if (obj_)
    return obj_->obj();
  else
    return 0;
}

template <class C>
const C& ptr<C>::operator*() const
{
  if (obj_)
    return *obj_->obj();
  else
    throw std::runtime_error("ptr: null dereference");
}

template <class C>
C *ptr<C>::modify() const
{
  if (obj_) {
    obj_->setDirty();
    return obj_->obj();
  } else
    throw std::runtime_error("ptr: null dereference");
}

template <class C>
bool ptr<C>::operator== (const ptr<C>& other) const
{
  return obj_ == other.obj_;
}

template <class C>
bool ptr<C>::operator!= (const ptr<C>& other) const
{
  return !(*this == other);
}

template <class C>
bool ptr<C>::operator< (const ptr<C>& other) const
{
  return obj_ < other.obj_;
}

template <class C>
ptr<C>::operator bool() const
{
  return obj_ != 0 && obj_->obj() != 0;
}

template <class C>
void ptr<C>::flush() const
{
  if (obj_)
    obj_->flush();
}

template <class C>
void ptr<C>::purge()
{
  if (obj_)
    obj_->purge();
}

template <class C>
void ptr<C>::reread()
{
  if (obj_)
    obj_->reread();
}

template <class C>
void ptr<C>::remove()
{
  if (obj_)
    obj_->remove();
}

template <class C>
typename dbo_traits<C>::IdType ptr<C>::id() const
{
  typedef typename dbo_traits<C>::IdType IdType;

  if (obj_)
    return obj_->id();
  else
    return dbo_traits<C>::invalidId();
}

template <class C>
int ptr<C>::version() const
{
  if (obj_)
    return obj_->version();
  else
    return -1;
}

template <class C>
ptr<C>::ptr(MetaDbo<C> *obj)
  : obj_(obj)
{
  takeObj();
}

template <class C>
void ptr<C>::transactionDone(bool success)
{
  obj_->transactionDone(success);
}

template <class C>
void ptr<C>::takeObj()
{
  if (obj_)
    obj_->incRef();
}

template <class C>
void ptr<C>::freeObj()
{
  if (obj_) {
    obj_->decRef();
    obj_ = 0;
  }
}

template <class C>
Dbo<C>::Dbo()
  : meta_(0)
{ }

template <class C>
typename dbo_traits<C>::IdType Dbo<C>::id() const
{
  if (meta_)
    return meta_->id();
  else
    return dbo_traits<C>::invalidId();
}

template <class C>
Session *Dbo<C>::session() const
{
  return meta_ ? meta_->session() : 0;
}

template <class C>
void query_result_traits< ptr<C> >
::getFields(Session& session, std::vector<std::string> *aliases,
	    std::vector<FieldInfo>& result)
{
  std::size_t first = result.size();
  session.getFields(session.tableName<C>(), result);

  if (aliases) {
    if (aliases->empty())
      throw std::logic_error("Session::query(): not enough aliases for result");

    std::string alias = aliases->front();
    aliases->erase(aliases->begin());

    for (std::size_t i = first; i < result.size(); ++i)
      result[i].setQualifier(alias, i == first);
  }
}

template <class C>
ptr<C> query_result_traits< ptr<C> >
::load(Session& session, SqlStatement& statement, int& column)
{
  return session.template load<C>(&statement, column);
}

template <class C>
void query_result_traits< ptr<C> >
::getValues(const ptr<C>& ptr, std::vector<boost::any>& values)
{
  ToAnysAction action(values);

  action.visit(ptr);
}

template <class C>
void query_result_traits< ptr<C> >
::setValue(const ptr<C>& ptr, int& index, const boost::any& value)
{
  FromAnyAction action(index, value);
  action.visit(ptr);
}

template <class C>
ptr<C> query_result_traits< ptr<C> >::create()
{
  return ptr<C>(new C());
}

template <class C>
void query_result_traits< ptr<C> >::add(Session& session, ptr<C>& ptr)
{
  session.add(ptr);
}

template <class C>
void query_result_traits< ptr<C> >::remove(ptr<C>& ptr)
{
  ptr.remove();
}

  }
}

#endif // WT_DBO_PTR_H_
