#ifndef DB_DB_H
#define DB_DB_H

#include <sqlpp11/table.h>
#include <sqlpp11/column_types.h>
#include <sqlpp11/char_sequence.h>

namespace db
{
  namespace Evolution_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "upTs";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "downTs";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "ups";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T ups;
            T& operator()() { return ups; }
            const T& operator()() const { return ups; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
    };
    struct Downs
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "downs";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T downs;
            T& operator()() { return downs; }
            const T& operator()() const { return downs; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
    };
  }

  struct Evolution: sqlpp::table_t<Evolution,
               Evolution_::Id,
               Evolution_::UpTs,
               Evolution_::DownTs,
               Evolution_::Ups,
               Evolution_::Downs>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "evolution";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "isOwn";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "name";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T name;
            T& operator()() { return name; }
            const T& operator()() const { return name; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
    };
    struct Owner
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "owner";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T owner;
            T& operator()() { return owner; }
            const T& operator()() const { return owner; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };
    struct Iban
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "iban";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T iban;
            T& operator()() { return iban; }
            const T& operator()() const { return iban; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
    };
    struct Bic
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "bic";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T bic;
            T& operator()() { return bic; }
            const T& operator()() const { return bic; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
    };
    struct AccountNumber
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "accountNumber";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T accountNumber;
            T& operator()() { return accountNumber; }
            const T& operator()() const { return accountNumber; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
    };
    struct BankCode
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "bankCode";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T bankCode;
            T& operator()() { return bankCode; }
            const T& operator()() const { return bankCode; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
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
    struct _alias_t
    {
      static constexpr const char _literal[] =  "account";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "date";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "fromId";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "toId";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "reference";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "amount";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "note";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "checked";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "internal";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
    struct _alias_t
    {
      static constexpr const char _literal[] =  "transfer";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "name";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T name;
            T& operator()() { return name; }
            const T& operator()() const { return name; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
  }

  struct Tag: sqlpp::table_t<Tag,
               Tag_::Id,
               Tag_::Name>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "tag";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "tagId";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "accountId";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
    struct _alias_t
    {
      static constexpr const char _literal[] =  "accountTag";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "tagId";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "transferId";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
    struct _alias_t
    {
      static constexpr const char _literal[] =  "transferTag";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
      struct _alias_t
      {
        static constexpr const char _literal[] =  "name";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T name;
            T& operator()() { return name; }
            const T& operator()() const { return name; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::can_be_null>;
    };
    struct HashedHeader
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "hashedHeader";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T hashedHeader;
            T& operator()() { return hashedHeader; }
            const T& operator()() const { return hashedHeader; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };
    struct Delimiter
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "delimiter";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T delimiter;
            T& operator()() { return delimiter; }
            const T& operator()() const { return delimiter; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
    };
    struct TextQualifier
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "textQualifier";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T textQualifier;
            T& operator()() { return textQualifier; }
            const T& operator()() const { return textQualifier; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
    };
    struct SkipFirstLine
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "skipFirstLine";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T skipFirstLine;
            T& operator()() { return skipFirstLine; }
            const T& operator()() const { return skipFirstLine; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::boolean>;
    };
    struct DateFormat
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "dateFormat";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T dateFormat;
            T& operator()() { return dateFormat; }
            const T& operator()() const { return dateFormat; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
    };
    struct ColumnPositions
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "columnPositions";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T columnPositions;
            T& operator()() { return columnPositions; }
            const T& operator()() const { return columnPositions; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };
    struct LineSuffix
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "lineSuffix";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T lineSuffix;
            T& operator()() { return lineSuffix; }
            const T& operator()() const { return lineSuffix; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };
  }

  struct Format: sqlpp::table_t<Format,
               Format_::Id,
               Format_::Name,
               Format_::HashedHeader,
               Format_::Delimiter,
               Format_::TextQualifier,
               Format_::SkipFirstLine,
               Format_::DateFormat,
               Format_::ColumnPositions,
               Format_::LineSuffix>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "format";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T format;
        T& operator()() { return format; }
        const T& operator()() const { return format; }
      };
    };
  };
  namespace File_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
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
    struct Path
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "path";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T path;
            T& operator()() { return path; }
            const T& operator()() const { return path; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text, sqlpp::tag::require_insert>;
    };
    struct FormatId
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "formatId";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T formatId;
            T& operator()() { return formatId; }
            const T& operator()() const { return formatId; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::require_insert>;
    };
    struct Watch
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "watch";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T watch;
            T& operator()() { return watch; }
            const T& operator()() const { return watch; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer>;
    };
    struct LastImport
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "lastImport";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T lastImport;
            T& operator()() { return lastImport; }
            const T& operator()() const { return lastImport; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::integer>;
    };
    struct LastImportHash
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "lastImportHash";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T lastImportHash;
            T& operator()() { return lastImportHash; }
            const T& operator()() const { return lastImportHash; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::text>;
    };
  }

  struct File: sqlpp::table_t<File,
               File_::Id,
               File_::Path,
               File_::FormatId,
               File_::Watch,
               File_::LastImport,
               File_::LastImportHash>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "file";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T file;
        T& operator()() { return file; }
        const T& operator()() const { return file; }
      };
    };
  };
}
#endif
