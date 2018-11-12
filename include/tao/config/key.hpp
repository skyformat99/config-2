// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_KEY_HPP
#define TAO_CONFIG_KEY_HPP

#include <sstream>
#include <vector>

#include "internal/grammar.hpp"

#include "part.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Rule >
         struct key_action
            : public json_pegtl::nothing< Rule >
         {
         };

         template<>
         struct key_action< rules::identifier >
         {
            template< typename Input >
            static void apply( const Input& in, std::vector< part >& st )
            {
               st.emplace_back( in.string() );
            }
         };

         template<>
         struct key_action< rules::index >
         {
            template< typename Input >
            static void apply( const Input& in, std::vector< part >& st )
            {
               st.emplace_back( std::stoul( in.string() ) );
            }
         };

         template<>
         struct key_action< rules::star >
         {
            static void apply0( std::vector< part >& st )
            {
               st.emplace_back( part::star );
            }
         };

         template<>
         struct key_action< rules::minus >
         {
            static void apply0( std::vector< part >& st )
            {
               st.emplace_back( part::minus );
            }
         };

      }  // namespace internal

      struct key
         : public std::vector< part >
      {
         key() noexcept = default;

         key( key&& ) noexcept = default;
         key& operator=( key&& ) noexcept = default;

         key( const key& ) = default;
         key& operator=( const key& ) = default;

         ~key() = default;

         explicit key( const std::string& s )
         {
            parse( s );
         }

         key( const std::initializer_list< part >& l )
            : std::vector< part >( l )
         {
         }

         key( const std::vector< part >::const_iterator& begin, const std::vector< part >::const_iterator& end )
            : std::vector< part >( begin, end )
         {
         }

         key& operator=( const std::string& s )
         {
            clear();
            parse( s );
            return *this;
         }

         key& operator=( const std::initializer_list< part >& l )
         {
            vector() = l;
            return *this;
         }

         void pop_front()
         {
            assert( !empty() );
            pop_front();
         }

         void pop_back()
         {
            assert( !empty() );
            pop_back();
         }

         std::vector< part >& vector() noexcept
         {
            return static_cast< std::vector< part >& >( *this );
         }

         const std::vector< part >& vector() const noexcept
         {
            return static_cast< const std::vector< part >& >( *this );
         }

         void parse( const std::string& s )
         {
            using grammar = json_pegtl::must< internal::rules::pointer, json_pegtl::eof >;  // TODO: Relax restriction on first part here?
            json_pegtl::memory_input< json_pegtl::tracking_mode::lazy, json_pegtl::eol::lf_crlf, const char* > in( s, __FUNCTION__ );
            json_pegtl::parse< grammar, internal::key_action >( in, vector() );
         }
      };

      inline key pop_front( const key& p )
      {
         assert( !p.empty() );
         return key( p.begin() + 1, p.end() );
      }

      inline key pop_back( const key& p )
      {
         assert( !p.empty() );
         return key( p.begin(), p.end() - 1 );
      }

      inline key& operator+=( key& l, const part::kind k )
      {
         l.emplace_back( k );
         return l;
      }

      inline key& operator+=( key& l, const std::size_t i )
      {
         l.emplace_back( i );
         return l;
      }

      inline key& operator+=( key& l, const std::string& n )
      {
         l.emplace_back( n );
         return l;
      }

      inline key operator+( const key& l, const part::kind k )
      {
         key r = l;
         r += k;
         return r;
      }

      inline key operator+( const key& l, const std::size_t i )
      {
         key r = l;
         r += i;
         return r;
      }

      inline key operator+( const key& l, const std::string& n )
      {
         key r = l;
         r += n;
         return r;
      }

      inline void key_to_stream( std::ostream& o, const key& p )
      {
         if( !p.empty() ) {
            part_to_stream( o, p[ 0 ] );

            for( std::size_t i = 1; i < p.size(); ++i ) {
               o << '.';
               part_to_stream( o, p[ i ] );
            }
         }
      }

      inline std::string key_to_string( const key& p )
      {
         std::ostringstream o;
         key_to_stream( o, p );
         return o.str();
      }

   }  // namespace config

}  // namespace tao

#endif
