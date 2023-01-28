/*
 Copyright (c) 2008-2018, Benoit AUTHEMAN All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the author or Destrat.io nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL AUTHOR BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//-----------------------------------------------------------------------------
// This file is a part of the GTpo software library.
//
// \file	gtpoGenNode.h
// \author	benoit@destrat.io
// \date	2016 01 22
//-----------------------------------------------------------------------------

#ifndef gtpoGenNode_h
#define gtpoGenNode_h

// STD headers
#include <list>
#include <unordered_set>
#include <memory>           // std::shared_ptr std::weak_ptr and std::make_shared
#include <functional>       // std::hash
#include <cassert>
#include <iterator>         // std::back_inserter

// GTpo headers
#include "./gtpoUtils.h"
#include "./gtpoGraphConfig.h"
#include "./gtpoNodeBehaviour.h"

namespace gtpo { // ::gtpo

template <class Config>
class GenGraph;

template <class Config>
class GenGroup;

template <class Config>
class GenEdge;

/*! \brief Base class for modelling nodes with an in/out edges list in a gtpo::GenGraph graph.
 *
 * \nosubgrouping
 */
template <class Config = gtpo::GraphConfig>
class GenNode : public Config::NodeBase,
                public gtpo::BehaviourableNode< gtpo::NodeBehaviour< Config >, std::tuple<> >,
                public std::enable_shared_from_this<GenNode<Config>>
                //public std::enable_shared_from_this<typename Config::FinalNode>
{
    friend GenGraph<Config>;   // GenGraph need access to setGraph()

    /*! \name Node Management *///---------------------------------------------
    //@{
public:
    using Graph         = GenGraph<Config>;
//    using Weak          = std::weak_ptr< typename Config::FinalNode >;
//    using Shared        = std::shared_ptr< typename Config::FinalNode >;
    using Weak          = std::weak_ptr<GenNode<Config> >;
    using Shared        = std::shared_ptr<GenNode<Config>>;
    using WeakNode      = Weak;
    using SharedNode    = Shared;
    using WeakNodes     = typename Config::template NodeContainer< WeakNode >;

    //! User friendly shortcut type to this concrete node Behaviourable base type.
    using BehaviourableBase = gtpo::Behaviourable< gtpo::NodeBehaviour< Config >, std::tuple<> >;
                                                   //typename Config::NodeBehaviours >;

    GenNode() noexcept : Config::NodeBase{} { }
    ~GenNode() {
        _inEdges.clear(); _outEdges.clear();
        _inNodes.clear(); _outNodes.clear();
        if ( _graph != nullptr ) {
            std::cerr << "gtpo::GenNode<>::~GenNode(): Warning: Node has been destroyed before beeing removed from the graph." << std::endl;
        }
        _graph = nullptr;
    }
    GenNode(const GenNode& node ) = delete;
    GenNode& operator=( GenNode const& ) = delete;
protected:
    inline  Graph*          getGraph() noexcept { return _graph; }
    inline  const Graph*    getGraph() const noexcept { return _graph; }
private:
    inline void             setGraph( Graph* graph ) noexcept { _graph = graph; }
public:
    Graph*                  _graph{ nullptr };
    //@}
    //-------------------------------------------------------------------------

    /*! \name Node Edges Management *///---------------------------------------
    //@{
public:
    //using WeakEdge      = typename GenEdge<Config>::Weak;
    using WeakEdge = std::weak_ptr<typename Config::FinalEdge>;
    //using SharedEdge    = typename GenEdge<Config>::Shared;
    using SharedEdge = std::shared_ptr<typename Config::FinalEdge>;
    using WeakEdges     = typename Config::template EdgeContainer< WeakEdge >;

    /*! \brief Insert edge \c outEdge as an out edge for this node.
     *
     * \note if \c outEdge source node is different from this node, it is set to this node.
     */
    auto    addOutEdge( WeakEdge sharedOutEdge ) noexcept( false ) -> void;
    /*! \brief Insert edge \c inEdge as an in edge for \c node.
     *
     * \note if \c inEdge destination node is different from \c node, it is automatically set to \c node.
     */
    auto    addInEdge( WeakEdge sharedInEdge ) noexcept( false ) -> void;
    /*! \brief Remove edge \c outEdge from this node out edges.
     *
     * \throw gtpo::bad_topology_error
     */
    auto    removeOutEdge( const WeakEdge outEdge ) noexcept( false ) -> void;
    /*! \brief Remove edge \c inEdge from this node in edges.
     *
     * \throw gtpo::bad_topology_error
     */
    auto    removeInEdge( const WeakEdge inEdge ) noexcept( false ) -> void;

    inline auto     getInEdges() const noexcept -> const WeakEdges& { return _inEdges; }
    inline auto     getOutEdges() const noexcept -> const WeakEdges& { return _outEdges; }

    inline auto     getInNodes() const noexcept -> const WeakNodes& { return _inNodes; }
    inline auto     getOutNodes() const noexcept -> const WeakNodes& { return _outNodes; }

    inline auto     getInDegree() const noexcept -> unsigned int { return static_cast<int>( _inEdges.size() ); }
    inline auto     getOutDegree() const noexcept -> unsigned int { return static_cast<int>( _outEdges.size() ); }
private:
    WeakEdges       _inEdges;
    WeakEdges       _outEdges;
    WeakNodes       _inNodes;
    WeakNodes       _outNodes;
    //@}
    //-------------------------------------------------------------------------

    /*! \name Node Edges Management *///---------------------------------------
    //@{
public:
    inline auto setGroup( const std::weak_ptr<typename Config::FinalGroup>& group ) noexcept -> void { _group = group; }
    inline auto getGroup( ) noexcept -> std::weak_ptr<typename Config::FinalGroup>& { return _group; }
    inline auto getGroup( ) const noexcept -> const std::weak_ptr<typename Config::FinalGroup>& { return _group; }
private:
    std::weak_ptr<typename Config::FinalGroup> _group;
    //@}
    //-------------------------------------------------------------------------
};

} // ::gtpo

#endif // gtpoGenNode_h

