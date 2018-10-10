/************************************************************************/
/*! \class RtAudioError
    \brief Exception handling class for RtAudio & RtMidi.

    The RtAudioError class is quite simple but it does allow errors to be
    "caught" by RtAudioError::Type. See the RtAudio and RtMidi
    documentation to know which methods can throw an RtAudioError.

*/
/************************************************************************/

#ifndef RTERROR_H
#define RTERROR_H

#include <exception>
#include <iostream>
#include <string>

class RtAudioError : public std::exception
{
 public:
  //! Defined RtAudioError types.
  enum Type {
    WARNING,           /*!< A non-critical error. */
    DEBUG_WARNING,     /*!< A non-critical error which might be useful for debugging. */
    UNSPECIFIED,       /*!< The default, unspecified error type. */
    NO_DEVICES_FOUND,  /*!< No devices found on system. */
    INVALID_DEVICE,    /*!< An invalid device ID was specified. */
    MEMORY_ERROR,      /*!< An error occured during memory allocation. */
    INVALID_PARAMETER, /*!< An invalid parameter was specified to a function. */
    INVALID_USE,       /*!< The function was called incorrectly. */
    DRIVER_ERROR,      /*!< A system driver error occured. */
    SYSTEM_ERROR,      /*!< A system error occured. */
    THREAD_ERROR       /*!< A thread error occured. */
  };

  //! The constructor.
  RtAudioError( const std::string& message, Type type = RtAudioError::UNSPECIFIED ) throw() : message_(message), type_(type) {}
 
  //! The destructor.
  virtual ~RtAudioError( void ) throw() {}

  //! Prints thrown error message to stderr.
  virtual void printMessage( void ) throw() { std::cerr << '\n' << message_ << "\n\n"; }

  //! Returns the thrown error message type.
  virtual const Type& getType(void) throw() { return type_; }

  //! Returns the thrown error message string.
  virtual const std::string& getMessage(void) throw() { return message_; }

  //! Returns the thrown error message as a c-style string.
  virtual const char* what( void ) const throw() { return message_.c_str(); }

 protected:
  std::string message_;
  Type type_;
};

#endif
