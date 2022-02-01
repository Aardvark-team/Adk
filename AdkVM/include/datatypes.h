/**
 * @file datatypes.h
 * @author Mason Marquez (theboys619@gmail.com)
 * @brief Possible datatypes for the Aardvark VM values
 * @version 0.1
 * @date 2021-12-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef DATATYPES_H
#define DATATYPES_H

namespace Aardvark {

  enum class DataTypes {
    None, // Aka 'Null'
    Int,
    Double,
    String,
    Object,
    Class
  };

}; // namespace Aardvark

#endif