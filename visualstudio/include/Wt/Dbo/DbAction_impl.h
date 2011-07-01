// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef WT_DBO_DBACTION_IMPL_H_
#define WT_DBO_DBACTION_IMPL_H_

#include <iostream>
#include <boost/lexical_cast.hpp>

namespace Wt {
  namespace Dbo {

template <class C, class Enable>
template <class A>
void persist<C, Enable>::apply(C& obj, A& action)
{
  obj.persist(action);
}

    /*
     * InitSchema
     */

template<class C>
void InitSchema::visit(C& obj)
{
  mapping_.surrogateIdFieldName = dbo_traits<C>::surrogateIdField();
  mapping_.versionFieldName = dbo_traits<C>::versionField();

  persist<C>::apply(obj, *this);
}

template<typename V>
void InitSchema::actId(V& value, const std::string& name, int size)
{
  mapping_.naturalIdFieldName = name;
  mapping_.naturalIdFieldSize = size;

  if (mapping_.surrogateIdFieldName)
    throw std::logic_error("Error: Wt::Dbo::id() called for class C "
			   "with surrogate key: "
			   "Wt::Dbo::dbo_traits<C>::surrogateIdField() != 0");

  idField_ = true;
  field(*this, value, name, size);
  idField_ = false;
}

template<typename V>
void InitSchema::act(const FieldRef<V>& field)
{
  if (idField_) {
    // Natural id
    mapping_.fields.push_back
      (FieldInfo(field.name(), &typeid(V), field.sqlType(session_),
		 FieldInfo::Mutable | FieldInfo::NeedsQuotes
		 | FieldInfo::NaturalId));
  } else if (!foreignKeyName_.empty())
    // Foreign key
    mapping_.fields.push_back
      (FieldInfo(field.name(), &typeid(V), field.sqlType(session_),
		 foreignKeyTable_, foreignKeyName_,
		 FieldInfo::Mutable | FieldInfo::NeedsQuotes
		 | FieldInfo::ForeignKey, fkConstraints_));
  else
    // Normal field
    mapping_.fields.push_back
      (FieldInfo(field.name(), &typeid(V), field.sqlType(session_),
		 FieldInfo::Mutable | FieldInfo::NeedsQuotes));
}

template<class C>
void InitSchema::actPtr(const PtrRef<C>& field)
{
  Session::Mapping<C> *mapping = session_.getMapping<C>();

  foreignKeyName_ = field.name();
  foreignKeyTable_ = mapping->tableName;
  fkConstraints_ = field.fkConstraints();

  field.visit(*this, &session_);

  foreignKeyName_.clear();
  foreignKeyTable_.clear();
  fkConstraints_ = 0;
}

template<class C>
void InitSchema::actCollection(const CollectionRef<C>& field)
{
  const char *joinTableName = session_.tableName<C>();

  mapping_.sets.push_back
    (Session::SetInfo(joinTableName, field.type(), field.joinName(),
		      field.joinId(), field.fkConstraints()));
}

    /*
     * DropSchema
     */

template<class C>
void DropSchema::visit(C& obj)
{
  persist<C>::apply(obj, *this);

  drop(tableName_);
}

template<typename V>
void DropSchema::actId(V& value, const std::string& name, int size)
{ }

template<typename V>
void DropSchema::act(const FieldRef<V>& field)
{ }

template<class C>
void DropSchema::actPtr(const PtrRef<C>& field)
{ }

template<class C>
void DropSchema::actCollection(const CollectionRef<C>& field)
{
  if (field.type() == ManyToMany) {
    if (tablesDropped_.count(field.joinName()) == 0)
      drop(field.joinName());
  } else {
    const char *tableName = session_.tableName<C>();
    if (tablesDropped_.count(tableName) == 0) {
      DropSchema action(session_, tableName, tablesDropped_);
      C dummy;
      action.visit(dummy);
    }
  }
}

    /*
     * DboAction
     */

template<class C>
void DboAction::actCollection(const CollectionRef<C>& field)
{
  if (dbo_->isPersisted()) {
    int statementIdx = Session::FirstSqlSelectSet + setStatementIdx_;

    const std::string& sql
      = dbo_->session()->getStatementSql(mapping_->tableName, statementIdx);

    field.value().setRelationData(dbo_->session(), &sql, dbo_);
  } else
    field.value().setRelationData(0, 0, 0);

  if (field.type() == ManyToOne)
    setStatementIdx_ += 1;
  else
    setStatementIdx_ += 3;
}

    /*
     * LoadDbAction
     */

template<typename V>
void LoadBaseAction::act(const FieldRef<V>& field)
{
  field.setValue(*dbo().session(), statement_, column_++);
}

template<class C>
void LoadBaseAction::actPtr(const PtrRef<C>& field)
{
  field.visit(*this, dbo().session());
}

template <class C>
LoadDbAction<C>::LoadDbAction(MetaDbo<C>& dbo, Session::Mapping<C>& mapping,
			      SqlStatement *statement, int& column)
  : LoadBaseAction(dbo, mapping, statement, column),
    dbo_(dbo)
{ }

template<class C>
void LoadDbAction<C>::visit(C& obj)
{
  ScopedStatementUse use(statement_);

  bool continueStatement = statement_ != 0;
  Session *session = dbo_.session();

  if (!continueStatement) {
    use(statement_ = session->template getStatement<C>(Session::SqlSelectById));
    statement_->reset();

    int column = 0;
    dbo_.bindId(statement_, column);

    statement_->execute();

    if (!statement_->nextRow()) {
      throw ObjectNotFoundException
	(boost::lexical_cast<std::string>(dbo_.id()));
    }
  }

  start();

  persist<C>::apply(obj, *this);

  if (!continueStatement && statement_->nextRow())
    throw Exception("Dbo load: multiple rows for id "
		    + boost::lexical_cast<std::string>(dbo_.id()) + " ??");

  if (continueStatement)
    use(0);
}

template<class C>
template<typename V>
void LoadDbAction<C>::actId(V& value, const std::string& name, int size)
{
  field(*this, value, name, size);

  dbo_.setId(value);
}

    /*
     * SaveDbAction
     */

template<class C>
void SaveBaseAction::actPtr(const PtrRef<C>& field)
{
  switch (pass_) {
  case Dependencies:
    field.value().flush();

    break;
  case Self:
    bindNull_ = !field.value();
    field.visit(*this, dbo().session());
    bindNull_ = false;

    break;
  case Sets:
    break;
  }
}

template<class C>
void SaveBaseAction::actCollection(const CollectionRef<C>& field)
{
  switch (pass_) {
  case Dependencies:
    break;

  case Self:
    if (isInsert_ || field.type() == ManyToMany)
      needSetsPass_ = true;

    break;
  case Sets:
    if (field.type() == ManyToMany) {
      typename collection< ptr<C> >::Activity *activity
	= field.value().activity();

      if (activity) {
	std::set< ptr<C> >& inserted = activity->inserted;

	// Sql insert
	int statementIdx
	  = Session::FirstSqlSelectSet + setStatementIdx() + 1;

	SqlStatement *statement;

	statement = dbo().session()->getStatement(mapping().tableName,
						  statementIdx);
	{
	  ScopedStatementUse use(statement);

	  for (typename std::set< ptr<C> >::iterator i = inserted.begin();
	       i != inserted.end(); ++i) {
	    // Make sure it is saved
	    i->flush();

	    statement->reset();
	    int column = 0;

	    dbo().bindId(statement, column);
	    i->obj()->bindId(statement, column);

	    statement->execute();
	  }
	}

	std::set< ptr<C> >& erased = activity->erased;

	// Sql delete
	++statementIdx;

	statement = dbo().session()->getStatement(mapping().tableName,
						  statementIdx);

	{
	  ScopedStatementUse use(statement);
	  for (typename std::set< ptr<C> >::iterator i = erased.begin();
	       i != erased.end(); ++i) {
	    // Make sure it is saved (?)
	    i->flush();

	    statement->reset();
	    int column = 0;

	    dbo().bindId(statement, column);
	    i->obj()->bindId(statement, column);

	    statement->execute();
	  }
	}

	activity->transactionInserted.insert(activity->inserted.begin(),
					     activity->inserted.end());
	activity->transactionErased.insert(activity->erased.begin(),
					   activity->erased.end());

	activity->inserted.clear();
	activity->erased.clear();
      }
    }

    DboAction::actCollection(field);
  }
}

template <class C>
SaveDbAction<C>::SaveDbAction(MetaDbo<C>& dbo, Session::Mapping<C>& mapping)
  : SaveBaseAction(dbo, mapping),
    dbo_(dbo)
{ }

template<class C>
void SaveDbAction<C>::visit(C& obj)
{
  /*
   * (1) Dependencies
   */
  startDependencyPass();

  persist<C>::apply(obj, *this);

  /*
   * (2) Self
   */
  {
    ScopedStatementUse use(statement_);
    if (!statement_) {
      isInsert_ = dbo_.deletedInTransaction()
	|| (dbo_.isNew() && !dbo_.savedInTransaction());

      use(statement_ = isInsert_
	  ? dbo_.session()->template getStatement<C>(Session::SqlInsert)
	  : dbo_.session()->template getStatement<C>(Session::SqlUpdate));
    } else
      isInsert_ = false;

    startSelfPass();
    persist<C>::apply(obj, *this);

    if (!isInsert_) {
      dbo_.bindId(statement_, column_);

      if (mapping().versionFieldName) {
	// when saved in the transaction, we will be at version() + 1
	statement_->bind(column_++, dbo_.version()
			 + (dbo_.savedInTransaction() ? 1 : 0));
      }
    }

    exec();

    if (!isInsert_) {
      int modifiedCount = statement_->affectedRowCount();
      if (modifiedCount != 1) {
	MetaDbo<C>& dbo = static_cast< MetaDbo<C>& >(dbo_);
	std::string idString = boost::lexical_cast<std::string>(dbo.id());

	throw StaleObjectException(idString, dbo_.version());
      }
    }
  }

  /*
   * (3) collections:
   *  - references in select queries (for ManyToOne and ManyToMany)
   *  - inserts in ManyToMany collections
   *  - deletes from ManyToMany collections
   */
  if (needSetsPass_) {
    startSetsPass();
    persist<C>::apply(obj, *this);
  }
}

template<class C>
template<typename V>
void SaveDbAction<C>::actId(V& value, const std::string& name, int size)
{
  field(*this, value, name, size);

  /* Later, we may also want to support id changes ? */
  if (pass_ == Self && isInsert_)
    dbo_.setId(value);
}

    /*
     * TransactionDoneAction
     */

template<class C>
void TransactionDoneAction::visit(C& obj)
{
  persist<C>::apply(obj, *this);
}

template<typename V>
void TransactionDoneAction::actId(V& value, const std::string& name, int size)
{ 
  field(*this, value, name, size);
}

template<typename V>
void TransactionDoneAction::act(const FieldRef<V>& field)
{ }

template<class C>
void TransactionDoneAction::actPtr(const PtrRef<C>& field)
{ }

template<class C>
void TransactionDoneAction::actCollection(const CollectionRef<C>& field)
{
  if (!success_)
    DboAction::actCollection(field);

  if (field.type() == ManyToMany) {
    if (success_)
      field.value().resetActivity();
    else {
      typename collection< ptr<C> >::Activity *activity
	= field.value().activity();

      if (activity) {
	activity->inserted = activity->transactionInserted;
	activity->transactionInserted.clear();
	activity->erased = activity->transactionErased;
	activity->transactionErased.clear();
      }
    }
  }
}

    /*
     * SessionAddAction
     */

template<class C>
void SessionAddAction::visit(C& obj)
{
  persist<C>::apply(obj, *this);
}

template<typename V>
void SessionAddAction::actId(V& value, const std::string& name, int size)
{ 
  field(*this, value, name, size);
}

template<typename V>
void SessionAddAction::act(const FieldRef<V>& field)
{ }

template<class C>
void SessionAddAction::actPtr(const PtrRef<C>& field)
{ }

template<class C>
void SessionAddAction::actCollection(const CollectionRef<C>& field)
{
  if (field.value().session() != session_)
    field.value().setRelationData(session_, 0, 0);

  // FIXME: cascade add ?
}

    /*
     * ToAnysAction
     */

template<class C>
void ToAnysAction::visit(const ptr<C>& obj)
{
  if (dbo_traits<C>::surrogateIdField())
    result_.push_back(obj.id());

  if (dbo_traits<C>::versionField())
    result_.push_back(obj.version());

  if (obj) {
    allEmpty_ = false;
    persist<C>::apply(const_cast<C&>(*obj), *this);
  } else {
    C dummy;
    allEmpty_ = true;
    persist<C>::apply(dummy, *this);
  }
}

template <typename V, class Enable = void>
struct ToAny
{
  static boost::any convert(const V& v) {
    return v;
  }  
};

template <typename Enum>
struct ToAny<Enum, typename boost::enable_if<boost::is_enum<Enum> >::type> 
{
  static boost::any convert(const Enum& v) {
    return static_cast<int>(v);
  }
};

template <typename V>
boost::any convertToAny(const V& v) {
  return ToAny<V>::convert(v);
}

template<typename V>
void ToAnysAction::actId(V& value, const std::string& name, int size)
{ 
  field(*this, value, name, size);
}

template<typename V>
void ToAnysAction::act(const FieldRef<V>& field)
{ 
  if (allEmpty_)
    result_.push_back(boost::any());
  else
    result_.push_back(convertToAny(field.value()));
}

template<class C>
void ToAnysAction::actPtr(const PtrRef<C>& field)
{
  field.visit(*this, 0);
}

template<class C>
void ToAnysAction::actCollection(const CollectionRef<C>& field)
{
}

    /*
     * FromAnyAction
     */

template<class C>
void FromAnyAction::visit(const ptr<C>& obj)
{
  if (dbo_traits<C>::surrogateIdField()) {
    if (index_ == 0)
      throw std::logic_error("dbo_result_traits::setValues(): "
			     "cannot set surrogate id.");
    --index_;
  }

  if (dbo_traits<C>::versionField()) {
    if (index_ == 0)
      throw std::logic_error("dbo_result_traits::setValues(): "
			     "cannot set version field.");
    --index_;
  }

  persist<C>::apply(const_cast<C&>(*obj), *this);

  if (index_ == -1)
    obj.modify();
}

template <typename V, class Enable = void>
struct FromAny
{
  static V convert(const boost::any& v) {
    return boost::any_cast<V>(v);
  }  
};

template <typename Enum>
struct FromAny<Enum, typename boost::enable_if<boost::is_enum<Enum> >::type> 
{
  static Enum convert(const boost::any& v) {
    return static_cast<Enum>(boost::any_cast<int>(v));
  }
};

template<typename V>
void FromAnyAction::act(const FieldRef<V>& field)
{
  if (index_ == 0) {
    field.setValue(FromAny<V>::convert(value_));

    index_ = -1;
  } else if (index_ > 0)
    --index_;
}

template<class C>
void FromAnyAction::actPtr(const PtrRef<C>& field)
{
  field.visit(*this, 0);
}

template<class C>
void FromAnyAction::actCollection(const CollectionRef<C>& field)
{
}

  }
}

#endif // WT_DBO_DBACTION_H_
