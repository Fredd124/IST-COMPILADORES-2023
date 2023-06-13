#ifndef __MML_TARGETS_SYMBOL_H__
#define __MML_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>
#include <cdk/types/functional_type.h>

namespace mml {

  class symbol {
    std::shared_ptr<cdk::basic_type> _type;
    std::string _name;
    long _value; // hack!
    bool _isFunction;
    std::string _label; // function label, if is function
    bool _isMain;
    int _offset;
    /* std::vector<std::shared_ptr<cdk::basic_type>> _argument_types; */
    bool _forward = false;
    bool _foreign = false;

  public:
    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, long value, bool isFunction = false, bool forward = false, bool foreign = false) :
        _type(type), _name(name), _value(value), _isFunction(isFunction), _forward(forward), _foreign(foreign) {
    }

    virtual ~symbol() {
      // EMPTY
    }

    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }
    void type(std::shared_ptr<cdk::basic_type> type) {
      _type = type;
    }
    bool is_typed(cdk::typename_type name) const {
      return _type->name() == name;
    }
    const std::string &name() const {
      return _name;
    }
    long value() const {
      return _value;
    }
    long value(long v) {
      return _value = v;
    }
    bool isFunction() const {
        return _isFunction;
    }
    void isFunction(bool isFunction) {
        _isFunction = isFunction;
    }
    const std::string &label() const {
      return _label;
    }
    void label(const std::string &label) {
      _label = label;
    }
    bool isMain() const {
      return _isMain;
    }
    void isMain(bool isMain) {
      _isMain = isMain;
    }
    int offset() const {
      return _offset;
    }
    void offset(int offset) {
      _offset = offset;
    }
    bool global() const {
      return _offset == 0;
    }
    bool forward() const {
      return _forward;
    }
    void forward(bool forward) {
      _forward = forward;
    }
    bool foreign() const {
      return _foreign;
    }
    void foreign(bool foreign) {
      _foreign = foreign;
    }
    std::vector<std::shared_ptr<cdk::basic_type>> argument_types() const {
      return cdk::functional_type::cast(_type)->input()->components();
    }
    std::shared_ptr<cdk::basic_type> argument_type(size_t i) const {
      return argument_types()[i];
    }
    void argument_type(size_t i, std::shared_ptr<cdk::basic_type> type) {
      argument_types()[i] = type;
    }
    size_t number_of_arguments() const {
      return argument_types().size();
    }
    bool is_argument_typed(size_t ax, cdk::typename_type name) const {
      return argument_types()[ax]->name() == name;
    }
    size_t argument_size(size_t ax) const {
      return argument_types()[ax]->size();
    }

  };
  inline auto make_symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name,
                          long value, bool isFunction, bool forward = false, bool foreign = false) {
    return std::make_shared<symbol>(type, name, value, isFunction, forward, foreign );
  }

} // mml

#endif
