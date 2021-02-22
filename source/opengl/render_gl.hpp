/*
 * Copyright (C) 2021 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/reshade#license
 */

#pragma once

#include "opengl.hpp"
#include "addon_manager.hpp"
#include <unordered_set>

namespace reshade::opengl
{
	api::resource_type convert_resource_type(GLenum target);
	api::resource_desc convert_resource_desc(GLsizeiptr buffer_size);
	api::resource_desc convert_resource_desc(api::resource_type type, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height = 1, GLsizei depth = 1);

	api::resource_view_dimension convert_resource_view_dimension(GLenum target);

	class device_impl : public api::api_object_impl<api::device, api::command_queue, api::command_list>
	{
	public:
		device_impl(HDC hdc, HGLRC hglrc);
		~device_impl();

		uint64_t get_native_object() const override { return reinterpret_cast<uintptr_t>(_hglrc); }

		reshade::api::render_api get_api() const override { return api::render_api::opengl; }

		bool check_format_support(uint32_t format, api::resource_usage usage) const override;

		bool check_resource_handle_valid(api::resource_handle resource) const override;
		bool check_resource_view_handle_valid(api::resource_view_handle view) const override;

		bool create_resource(api::resource_type type, const api::resource_desc &desc, api::resource_usage initial_state, api::resource_handle *out_resource) override;
		bool create_resource_view(api::resource_handle resource, api::resource_view_type type, const api::resource_view_desc &desc, api::resource_view_handle *out_view) override;

		void destroy_resource(api::resource_handle resource) override;
		void destroy_resource_view(api::resource_view_handle view) override;

		void get_resource_from_view(api::resource_view_handle view, api::resource_handle *out_resource) const override;

		api::resource_desc get_resource_desc(api::resource_handle resource) const override;

		void wait_idle() const override;

		api::resource_view_handle get_depth_stencil_from_fbo(GLuint fbo) const;
		api::resource_view_handle get_render_target_from_fbo(GLuint fbo, GLuint drawbuffer) const;

		api::device *get_device() override { return this; }

		api::command_list *get_immediate_command_list() override { return this; }

		void flush_immediate_command_list() const override;

		void draw(uint32_t vertices, uint32_t instances, uint32_t first_vertex, uint32_t first_instance) override;
		void draw_indexed(uint32_t indices, uint32_t instances, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) override;

		void copy_resource(api::resource_handle source, api::resource_handle destination) override;

		void transition_state(api::resource_handle, api::resource_usage, api::resource_usage) override { /* no-op */ }

		void clear_depth_stencil_view(api::resource_view_handle dsv, uint32_t clear_flags, float depth, uint8_t stencil) override;
		void clear_render_target_view(api::resource_view_handle rtv, const float color[4]) override;

	public:
		bool _compatibility_context = false;
		const HGLRC _hglrc;
		std::unordered_set<HDC> _hdcs;
		GLuint _current_vertex_count = 0; // Used to calculate vertex count inside glBegin/glEnd pairs

	protected:
		// Cached context information for quick access
		GLuint _default_fbo_width = 0;
		GLuint _default_fbo_height = 0;
		GLenum _default_color_format = GL_NONE;
		GLenum _default_depth_format = GL_NONE;

	private:
		GLuint _copy_fbo[2] = {};
	};
}
