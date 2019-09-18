#ifndef MARLIN_PARAMETER_h
#define MARLIN_PARAMETER_h 1

// -- std headers
#include <iostream>
#include <string>
#include <sstream>
#include <typeinfo>
#include <memory>
#include <vector>

// -- marlin headers
#include <marlin/Utils.h>
#include <marlin/StringParameters.h>

namespace marlin {

  /**
   *  @brief  Parameter class.
   *  Holds a steering variable.
   *
   *  @author F. Gaede, DESY
   *  @author R. Ete, DESY
   */
  class Parameter {
    friend std::ostream& operator<< (  std::ostream& , Parameter& ) ;

  public:
    Parameter() = default ;
    virtual ~Parameter() = default ;

    /**
     *  @brief  Get the parameter name
     */
    const std::string& name() const ;

    /**
     *  @brief  Get the parameter description
     */
    const std::string& description() const ;

    /**
     *  @brief  Get the set size (if the parameter is a vector)
     */
    int setSize() const ;

    /**
     *  @brief  Whether the parameter is optional
     */
    bool isOptional() const ;

    /**
     *  @brief  Whether the value has been set during steering file parsing
     */
    bool valueSet() const ;

    /**
     *  @brief  Get the parameter type as string
     */
    virtual const std::string type() const = 0 ;

    /**
     *  @brief  Get the parameter value as string
     */
    virtual const std::string value() const = 0 ;

    /**
     *  @brief  Get the parameter default value
     */
    virtual const std::string defaultValue() const = 0 ;

    /**
     *  @brief  Set the value using the string parameters
     *
     *  @param  params the string parameters to get the parameter value from
     */
    virtual void setValue( StringParameters* params ) = 0 ;

  protected:
    /// The parameter description
    std::string       _description {""} ;
    /// The parameter name
    std::string       _name {""} ;
    /// The set size, if the parameter type is vector
    int               _setSize {0} ;
    /// Whether the parameter is optional
    bool              _optional {false} ;
    /// Whether the value has been set after parsing
    bool              _valueSet {false} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  /**
   *  @brief  ParameterT template class.
   *  Templated implementation of Parameter class.
   */
  template <typename T>
  class ParameterT : public Parameter {
  public:
    ~ParameterT() = default ;

  public:
    /**
     *  @brief  Constructor
     *
     *  @param  parameterName the parameter name
     *  @param  parameterDescription the parameter description
     *  @param  parameter the parameter value reference
     *  @param  parameterDefaultValue the parameter default value
     *  @param  optional whether the parameter is optional while parsing the steering file
     *  @param  parameterSetSize the set size of the parameter, if of type vector
     */
    ParameterT( const std::string& parameterName, const std::string& parameterDescription,
        T& parameter, const T& parameterDefaultValue,
        bool optional, int parameterSetSize = 0) ;

    /**
     *  @brief  Get the parameter value
     */
    const T &valueT() const ;

    /**
     *  @brief  Get the parameter default value
     */
    const T &defaultValueT() const ;

  public:
    // interface implementation
    const std::string type() const override ;
    const std::string defaultValue() const override ;
    const std::string value() const override ;
    void setValue( StringParameters* params ) override ;

  protected:
    /// The parameter value reference
    T  &_value ;
    /// The default parameter value reference
    T   _defaultValue ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  class Parametrized {
  public:
    typedef std::shared_ptr<Parameter>            ParameterPtr ;
    typedef std::map<std::string, ParameterPtr>   ParameterMap ;
    typedef std::map<std::string, std::string>    LCIOTypeMap ;
    typedef ParameterMap::iterator                iterator ;
    typedef ParameterMap::const_iterator          const_iterator ;

  public:
    Parametrized() = default ;
    virtual ~Parametrized() = default ;
    Parametrized(const Parametrized &) = delete ;
    Parametrized &operator=(const Parametrized &) = delete ;

   /**
    *  @brief  Register a steering variable.
    *  The default value has to be of the _same_ type as the parameter, e.g.
    *  @code{cpp}
    *  float _cut ;
    *  // ...
    *  registerParameter( "Cut", "cut...", _cut , float( 3.141592 ) ) ;
    *  @endcode
    *  as implicit conversions don't work for templates.<br>
    *  The optional parameter setSize is used for formating the printout of parameters.
    *  This can be used if the parameter values are expected to come in sets of fixed size.
    */
    template<class T>
    void registerParameter(const std::string& parameterName,
          const std::string& parameterDescription,
		      T& parameter,
          const T& defaultVal,
          int setSize = 0 ) ;

   /**
    *  @brief  Same as registerParameter except that the parameter is optional.
    *  The value of the parameter will still be set to the default value, which
    *  is used to print an example steering line.
    *  Use parameterSet() to check whether it actually has been set in the steering
    *  file.
    */
    template<class T>
    void registerOptionalParameter(const std::string& parameterName,
				   const std::string& parameterDescription,
				   T& parameter,
				   const T& defaultVal,
				   int setSize = 0 ) ;
    
   /**
    *  @brief  Specialization of registerParameter() for a parameter that defines an
    *  input collection - can be used fo checking the consistency of the steering file.
    */
    void registerInputCollection(const std::string& collectionType,
          const std::string& parameterName,
          const std::string& parameterDescription,
          std::string& parameter,
          const std::string& defaultVal,
          int setSize = 0 ) ;
    
    /**
     *  @brief  Specialization of registerParameter() for a parameter that defines one or several
     *  input collections - can be used fo checking the consistency of the steering file.
     */
    void registerInputCollections(const std::string& collectionType,
          const std::string& parameterName,
          const std::string& parameterDescription,
          std::vector<std::string>& parameter,
          const std::vector<std::string>& defaultVal,
          int setSize = 0 ) ;
    
    /**
     *  @brief  Specialization of registerParameter() for a parameter that defines an
     *  output collection - can be used fo checking the consistency of the steering file.
     */
    void registerOutputCollection(const std::string& collectionType,
				  const std::string& parameterName,
				  const std::string& parameterDescription,
				  std::string& parameter,
				  const std::string& defaultVal,
				  int setSize = 0 ) ;

    /**
     *  @brief  Tests whether the parameter has been set in the steering file
     */
    bool parameterSet( const std::string& name ) ;

    /**
     *  @brief  Clear the parameter map
     */
    void clearParameters() ;

    /**
     *  @brief  Update the parameter map with the input parameters
     *
     *  @param  parameters the input parameter list
     */
    void setParameters( std::shared_ptr<StringParameters> parameters ) ;

    /**
     *  @brief  Get the list of parameter names
     */
    std::vector<std::string> parameterNames() const ;

    /**
     *  @brief  Get a parameter value.
     *  If the parameter exists, an attempt to cast the parameter type
     *  to the same type as it was registered is done.
     *  It it fails, an exception is thrown.
     *  If the parameter doesn't exists, an exception is thrown.
     *  If the parameter exists but is not set, the default value is returned.
     *
     *  @param  name the parameter name
     */
    template <typename T>
    T getParameter( const std::string& name ) const ;
    
    /**
     *  @brief  Return the LCIO input type for the collection colName - empty string if colName is
     *  not a registered collection name
     */
    std::string getLCIOInType( const std::string& colName ) const ;

    /**
     *  @brief  Return the LCIO output type for the collection colName - empty string if colName is
     *  not a registered collection name
     */
    std::string getLCIOOutType( const std::string& colName ) const ;
    
    /**
     *  @brief  True if the given parameter defines an LCIO input collection, i.e. the type has
     *  been defined with setLCIOInType().
     */
    bool isInputCollectionName( const std::string& parameterName ) const ;

    /**
     *  @brief   True if the given parameter defines an LCIO output collection
     */
    bool isOutputCollectionName( const std::string& parameterName ) const ;
    
    /**
     *  @brief  Begin iterator to parameter map
     */
    iterator pbegin() ;
    
    /**
     *  @brief  End iterator of parameter map
     */
    iterator pend() ;
    
    /**
     *  @brief  Begin iterator to parameter map
     */
    const_iterator pbegin() const ;
    
    /**
     *  @brief  End iterator of parameter map
     */
    const_iterator pend() const ;

  private:
    /**
     *  @brief  Tests whether the parameter has been registered before
     *
     *  @param name name of the parameter to check
     *  @throw logic_error if parameter has been registered before
     */
    void checkForExistingParameter( const std::string& parameterName ) const ;
    
    /** 
     *  @brief  Set the expected LCIO type for a parameter that refers to 
     *  one or more input collections.
     */
    void setLCIOInType(const std::string& colName,  const std::string& lcioInType) ;

    /**  
     *  @brief  Set the LCIO type for a parameter that refers to an output collections,
     *  i.e. the type has been defined with setLCIOOutType().
     */
    void setLCIOOutType(const std::string& collectionName,  const std::string& lcioOutType) ;

  private:
    ///< The parameter map
    ParameterMap                       _parameters {} ;
    ///< The input collection information
    LCIOTypeMap                        _inTypeMap {} ;
    ///< The output collection information
    LCIOTypeMap                        _outTypeMap {} ;
  };


  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <typename T>
  inline ParameterT<T>::ParameterT( const std::string& parameterName, const std::string& parameterDescription,
      T& parameter, const T& parameterDefaultValue,
      bool optional, int parameterSetSize ) :
    _value( parameter ),
    _defaultValue( parameterDefaultValue ) {
    _name = parameterName ;
    _value = parameterDefaultValue ;
    _description = parameterDescription ;
    _optional = optional ;
    _valueSet = false ;
    _setSize = parameterSetSize ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline const T &ParameterT<T>::valueT() const {
    return _value ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline const T &ParameterT<T>::defaultValueT() const {
    return _defaultValue ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline const std::string ParameterT<T>::type() const {
    // make this human readable
    if     ( typeid( _value ) == typeid( std::vector<int> )) return "IntVec" ;
    else if( typeid( _value ) == typeid( std::vector<float>  )) return "FloatVec" ;
    else if( typeid( _value ) == typeid( std::vector<std::string> )) return "StringVec" ;
    else if( typeid( _value ) == typeid( int   )) return "int" ;
    else if( typeid( _value ) == typeid( float )) return "float" ;
    else if( typeid( _value ) == typeid( double )) return "double" ;
    else if( typeid( _value ) == typeid( std::string ) ) return "string" ;
    else if( typeid( _value ) == typeid( bool ) ) return "bool";
    else return typeid( _value ).name() ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline const std::string ParameterT<T>::defaultValue() const {
    return StringUtil::join( _defaultValue, " " ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline const std::string ParameterT<T>::value() const {
    return StringUtil::join( _value, " " ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  void ParameterT<T>::setValue( StringParameters* params ) {
    if( params->isParameterSet( name() ) ) {
      _valueSet = true ;
    }
    params->get( name(), _value ) ;
  }

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <class T>
  inline void Parametrized::registerParameter(const std::string& parameterName,
        const std::string& parameterDescription,
        T& parameter,
        const T& defaultVal,
        int setSize ) {
    checkForExistingParameter( parameterName );
    _parameters[ parameterName ] = std::make_shared<ParameterT<T>>( parameterName , parameterDescription,
            parameter, defaultVal,
            false , setSize) ;
  }

  //--------------------------------------------------------------------------

  template <class T>
  inline void Parametrized::registerOptionalParameter(const std::string& parameterName,
         const std::string& parameterDescription,
         T& parameter,
         const T& defaultVal,
         int setSize ) {
     checkForExistingParameter( parameterName );
     _parameters[ parameterName ] = std::make_shared<ParameterT<T>>( parameterName , parameterDescription,
             parameter, defaultVal,
             true , setSize) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline T Parametrized::getParameter( const std::string& name ) const {
    auto parameter = _parameters.find( name ) ;
    if (parameter == _parameters.end() ) {
      throw Exception( "Parameter " + name + " not found!" ) ;
    }
    auto parameterT = std::static_pointer_cast<ParameterT<T>>( parameter->second ) ;
    if ( nullptr == parameterT ) {
      throw Exception( "Wrong parameter cast !" ) ;
    }
    return (parameterT->valueSet() ? parameterT->valueT() : parameterT->defaultValueT() ) ;
  }

} // end namespace marlin
#endif
