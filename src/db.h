#ifndef DB_DB_H
#define DB_DB_H

#include <sqlpp11/table.h>
#include <sqlpp11/column_types.h>

namespace db
{
  namespace Evolution_
  {
    struct Id
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "id"; }
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct UpTs
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "upTs"; }
        template<typename T>
        struct _member_t
          {
            T upTs;
            T& operator()() { return upTs; }
            const T& operator()() const { return upTs; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer>;
    };
    struct DownTs
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "downTs"; }
        template<typename T>
        struct _member_t
          {
            T downTs;
            T& operator()() { return downTs; }
            const T& operator()() const { return downTs; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer>;
    };
    struct Ups
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "ups"; }
        template<typename T>
        struct _member_t
          {
            T ups;
            T& operator()() { return ups; }
            const T& operator()() const { return ups; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };
    struct Downs
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "downs"; }
        template<typename T>
        struct _member_t
          {
            T downs;
            T& operator()() { return downs; }
            const T& operator()() const { return downs; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };
  }

  struct Evolution: sqlpp::table_t<Evolution,
               Evolution_::Id,
               Evolution_::UpTs,
               Evolution_::DownTs,
               Evolution_::Ups,
               Evolution_::Downs>
  {
    struct _name_t
    {
      static constexpr const char* _get_name() { return "evolution"; }
      template<typename T>
      struct _member_t
      {
        T evolution;
        T& operator()() { return evolution; }
        const T& operator()() const { return evolution; }
      };
    };
  };
  namespace Account_
  {
    struct Id
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "id"; }
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct IsOwn
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "isOwn"; }
        template<typename T>
        struct _member_t
          {
            T isOwn;
            T& operator()() { return isOwn; }
            const T& operator()() const { return isOwn; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::boolean>;
    };
    struct Name
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "name"; }
        template<typename T>
        struct _member_t
          {
            T name;
            T& operator()() { return name; }
            const T& operator()() const { return name; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar>;
    };
    struct Owner
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "owner"; }
        template<typename T>
        struct _member_t
          {
            T owner;
            T& operator()() { return owner; }
            const T& operator()() const { return owner; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Iban
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "iban"; }
        template<typename T>
        struct _member_t
          {
            T iban;
            T& operator()() { return iban; }
            const T& operator()() const { return iban; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar>;
    };
    struct Bic
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "bic"; }
        template<typename T>
        struct _member_t
          {
            T bic;
            T& operator()() { return bic; }
            const T& operator()() const { return bic; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar>;
    };
    struct AccountNumber
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "accountNumber"; }
        template<typename T>
        struct _member_t
          {
            T accountNumber;
            T& operator()() { return accountNumber; }
            const T& operator()() const { return accountNumber; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar>;
    };
    struct BankCode
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "bankCode"; }
        template<typename T>
        struct _member_t
          {
            T bankCode;
            T& operator()() { return bankCode; }
            const T& operator()() const { return bankCode; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar>;
    };
  }

  struct Account: sqlpp::table_t<Account,
               Account_::Id,
               Account_::IsOwn,
               Account_::Name,
               Account_::Owner,
               Account_::Iban,
               Account_::Bic,
               Account_::AccountNumber,
               Account_::BankCode>
  {
    struct _name_t
    {
      static constexpr const char* _get_name() { return "account"; }
      template<typename T>
      struct _member_t
      {
        T account;
        T& operator()() { return account; }
        const T& operator()() const { return account; }
      };
    };
  };
  namespace Transfer_
  {
    struct Id
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "id"; }
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Date
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "date"; }
        template<typename T>
        struct _member_t
          {
            T date;
            T& operator()() { return date; }
            const T& operator()() const { return date; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct FromId
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "fromId"; }
        template<typename T>
        struct _member_t
          {
            T fromId;
            T& operator()() { return fromId; }
            const T& operator()() const { return fromId; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct ToId
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "toId"; }
        template<typename T>
        struct _member_t
          {
            T toId;
            T& operator()() { return toId; }
            const T& operator()() const { return toId; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct Reference
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "reference"; }
        template<typename T>
        struct _member_t
          {
            T reference;
            T& operator()() { return reference; }
            const T& operator()() const { return reference; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };
    struct Amount
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "amount"; }
        template<typename T>
        struct _member_t
          {
            T amount;
            T& operator()() { return amount; }
            const T& operator()() const { return amount; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct Note
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "note"; }
        template<typename T>
        struct _member_t
          {
            T note;
            T& operator()() { return note; }
            const T& operator()() const { return note; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
    };
    struct Checked
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "checked"; }
        template<typename T>
        struct _member_t
          {
            T checked;
            T& operator()() { return checked; }
            const T& operator()() const { return checked; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::boolean>;
    };
    struct Internal
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "internal"; }
        template<typename T>
        struct _member_t
          {
            T internal;
            T& operator()() { return internal; }
            const T& operator()() const { return internal; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::boolean>;
    };
  }

  struct Transfer: sqlpp::table_t<Transfer,
               Transfer_::Id,
               Transfer_::Date,
               Transfer_::FromId,
               Transfer_::ToId,
               Transfer_::Reference,
               Transfer_::Amount,
               Transfer_::Note,
               Transfer_::Checked,
               Transfer_::Internal>
  {
    struct _name_t
    {
      static constexpr const char* _get_name() { return "transfer"; }
      template<typename T>
      struct _member_t
      {
        T transfer;
        T& operator()() { return transfer; }
        const T& operator()() const { return transfer; }
      };
    };
  };
  namespace Tag_
  {
    struct Id
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "id"; }
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Name
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "name"; }
        template<typename T>
        struct _member_t
          {
            T name;
            T& operator()() { return name; }
            const T& operator()() const { return name; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::can_be_null>;
    };
  }

  struct Tag: sqlpp::table_t<Tag,
               Tag_::Id,
               Tag_::Name>
  {
    struct _name_t
    {
      static constexpr const char* _get_name() { return "tag"; }
      template<typename T>
      struct _member_t
      {
        T tag;
        T& operator()() { return tag; }
        const T& operator()() const { return tag; }
      };
    };
  };
  namespace AccountTag_
  {
    struct TagId
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "tagId"; }
        template<typename T>
        struct _member_t
          {
            T tagId;
            T& operator()() { return tagId; }
            const T& operator()() const { return tagId; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct AccountId
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "accountId"; }
        template<typename T>
        struct _member_t
          {
            T accountId;
            T& operator()() { return accountId; }
            const T& operator()() const { return accountId; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
  }

  struct AccountTag: sqlpp::table_t<AccountTag,
               AccountTag_::TagId,
               AccountTag_::AccountId>
  {
    struct _name_t
    {
      static constexpr const char* _get_name() { return "accountTag"; }
      template<typename T>
      struct _member_t
      {
        T accountTag;
        T& operator()() { return accountTag; }
        const T& operator()() const { return accountTag; }
      };
    };
  };
  namespace TransferTag_
  {
    struct TagId
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "tagId"; }
        template<typename T>
        struct _member_t
          {
            T tagId;
            T& operator()() { return tagId; }
            const T& operator()() const { return tagId; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct TransferId
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "transferId"; }
        template<typename T>
        struct _member_t
          {
            T transferId;
            T& operator()() { return transferId; }
            const T& operator()() const { return transferId; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
  }

  struct TransferTag: sqlpp::table_t<TransferTag,
               TransferTag_::TagId,
               TransferTag_::TransferId>
  {
    struct _name_t
    {
      static constexpr const char* _get_name() { return "transferTag"; }
      template<typename T>
      struct _member_t
      {
        T transferTag;
        T& operator()() { return transferTag; }
        const T& operator()() const { return transferTag; }
      };
    };
  };
  namespace Format_
  {
    struct Id
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "id"; }
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
    };
    struct Name
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "name"; }
        template<typename T>
        struct _member_t
          {
            T name;
            T& operator()() { return name; }
            const T& operator()() const { return name; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::can_be_null>;
    };
    struct Delimiter
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "delimiter"; }
        template<typename T>
        struct _member_t
          {
            T delimiter;
            T& operator()() { return delimiter; }
            const T& operator()() const { return delimiter; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::can_be_null>;
    };
    struct TextQualifier
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "textQualifier"; }
        template<typename T>
        struct _member_t
          {
            T textQualifier;
            T& operator()() { return textQualifier; }
            const T& operator()() const { return textQualifier; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::can_be_null>;
    };
    struct ColumnsOrder
    {
      struct _name_t
      {
        static constexpr const char* _get_name() { return "columnsOrder"; }
        template<typename T>
        struct _member_t
          {
            T columnsOrder;
            T& operator()() { return columnsOrder; }
            const T& operator()() const { return columnsOrder; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
  }

  struct Format: sqlpp::table_t<Format,
               Format_::Id,
               Format_::Name,
               Format_::Delimiter,
               Format_::TextQualifier,
               Format_::ColumnsOrder>
  {
    struct _name_t
    {
      static constexpr const char* _get_name() { return "format"; }
      template<typename T>
      struct _member_t
      {
        T format;
        T& operator()() { return format; }
        const T& operator()() const { return format; }
      };
    };
  };
}
#endif
