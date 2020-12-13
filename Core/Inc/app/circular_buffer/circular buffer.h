/*
 * circular buffer.h
 *
 *  Created on: 1 gru 2020
 *      Author: theoar
 */

#ifndef INC_APP_CIRCULAR_BUFFER_CIRCULAR_BUFFER_H_
#define INC_APP_CIRCULAR_BUFFER_CIRCULAR_BUFFER_H_

namespace circular_buffer
{
  template<int size, typename T>
  class CircularBuffer
  {
  private:
      T vector[size];
      int readIndex = 0;
      int writeIndex = 0;

      bool empty = true;

  public:
      CircularBuffer() = default;

      void add(const T & ref)
      {
          if(!this->isEmpty())
          {
              if(this->writeIndex==this->readIndex)
                  this->readIndex++;
          }

          this->empty = false;

          this->vector[this->writeIndex] = ref;
          this->writeIndex++;

          this->writeIndex %= size;
          this->readIndex %= size;
      }

      bool isEmpty() { return this->empty; }

      T pop(bool & ok)
      {
          ok = !this->isEmpty();

          if(ok)
          {
              int toGetIndex = this->readIndex;

              this->readIndex++;

              this->readIndex %= size;

              if(this->readIndex==this->writeIndex)
                  this->empty = true;

              return this->vector[toGetIndex];
          }
          else
          {
              return T();
          }
      }

      T pop()
      {
	bool ok = false;
	return this->pop(ok);
      }
  };
//  template<int size, typename T>
//    class CircularBuffer
//    {
//      private:
//	T vector[size];
//	int readIndex = -1;
//	int writeIndex = 0;
//
//      public:
//	CircularBuffer() = default;
//
//	void add(const T &ref)
//	{
//	  if(this->writeIndex == this->readIndex)
//	    this->readIndex++;
//
//	  this->vector[this->writeIndex] = ref;
//	  this->writeIndex++;
//
//	  this->writeIndex %= size;
//
//	  if(this->readIndex == -1)
//	    this->readIndex = 0;
//
//	  this->readIndex %= size;
//	}
//
//	bool isEmpty()
//	{
//	  return this->readIndex == -1;
//	}
//
//	T pop(bool &ok)
//	{
//	  ok = !this->isEmpty();
//
//	  if(ok)
//	  {
//	    int toGetIndex = this->readIndex;
//
//	    this->readIndex++;
//
//	    this->readIndex %= size;
//
//	    if(this->readIndex == this->writeIndex)
//	      this->readIndex = -1;
//
//	    if(this->readIndex == -1)
//	      this->writeIndex = 0;
//
//	    return this->vector[toGetIndex];
//	  }
//	  else
//	  {
//	    return T();
//	  }
//	}
//    };
}

#endif /* INC_APP_CIRCULAR_BUFFER_CIRCULAR_BUFFER_H_ */
