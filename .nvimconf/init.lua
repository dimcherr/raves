vim.g.mapleader = " "
vim.g.maplocalleader = "\\"
vim.opt.clipboard = "unnamedplus"
vim.cmd([[
cnoreabbrev <expr> w (getcmdtype() == ':' && getcmdline() == 'w') ? 'update' : 'w'
]])

-- Lazy
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"
if not (vim.uv or vim.loop).fs_stat(lazypath) then
  local lazyrepo = "https://github.com/folke/lazy.nvim.git"
  local out = vim.fn.system({ "git", "clone", "--filter=blob:none", "--branch=stable", lazyrepo, lazypath })
  if vim.v.shell_error ~= 0 then
    vim.api.nvim_echo({
      { "Failed to clone lazy.nvim:\n", "ErrorMsg" },
      { out, "WarningMsg" },
      { "\nPress any key to exit..." },
    }, true, {})
    vim.fn.getchar()
    os.exit(1)
  end
end
vim.opt.rtp:prepend(lazypath)

require("lazy").setup({
    spec = {
        {
          "skywind3000/asyncrun.vim",
          config = function()
            -- Open output window on the right:
            vim.g.asyncrun_open = 20      -- height/width
            vim.g.asyncrun_bell = 1
          end
        },
        {
            'tpope/vim-dispatch',
        },
        {
            "sphamba/smear-cursor.nvim",
            opts = {
            smear_between_buffers = true,
            smear_between_neighbor_lines = true,
            scroll_buffer_space = true,
            legacy_computing_symbols_support = false,
            smear_insert_mode = true,
            },
        },
        {
          "NeogitOrg/neogit",
          lazy = true,
          dependencies = {
            "nvim-lua/plenary.nvim",
            "sindrets/diffview.nvim",
            "nvim-telescope/telescope.nvim",
          },
          cmd = "Neogit",
          keys = {
            { "<leader>gg", "<cmd>Neogit<cr>", desc = "Show Neogit UI" }
          }
        },
        {
            "folke/tokyonight.nvim",
            lazy = false,
            priority = 1000,
            opts = {},
        },
        {
            "nvim-telescope/telescope.nvim",
            tag = '0.1.5',
            dependencies = { 'nvim-lua/plenary.nvim' },
        },
        {
            "nvim-treesitter/nvim-treesitter",
            branch = 'master',
            lazy = false,
            build = ":TSUpdate",
        },
        {
            "nvim-neo-tree/neo-tree.nvim",
            branch = "v3.x",
            dependencies = {
              "nvim-lua/plenary.nvim",
              "MunifTanjim/nui.nvim",
              "nvim-tree/nvim-web-devicons",
            },
        },
        {
            "mason-org/mason.nvim",
            opts = {}
        },
        {
            "mason-org/mason-lspconfig.nvim"
        },
        {
            "neovim/nvim-lspconfig",
            lazy = false,
            dependencies = {
                { "ms-jpq/coq_nvim", branch = "coq" },
            },
            init = function()
                vim.g.coq_settings = {
                    auto_start = true, -- if you want to start COQ at startup
                    completion = {
                        always = true,
                    },
                }
            end,
        }
    },
})

-- Telescope
require('telescope').setup{
    defaults = {
        mappings = {
            i = {
                ["<C-j>"] = require('telescope.actions').move_selection_next,
                ["<C-k>"] = require('telescope.actions').move_selection_previous,
            },
            n = {
                ["j"] = require('telescope.actions').move_selection_next,
                ["k"] = require('telescope.actions').move_selection_previous,
            },
        },
        layout_strategy = 'horizontal',
        layout_config = {
          horizontal = {
            preview_cutoff = 1,
            width = 0.75,
            height = 0.75,
            preview_width = 0.5,
            anchor = 'SE'
          },
        },
    },
}

-- Neotree
require('neo-tree').setup({
  filesystem = {
    bind_to_cwd = true,
    follow_current_file = {
      enabled = true,
    },
    use_libuv_file_watcher = true,
  },
  window = {
      width = 40,
  },
})

-- Treesitter
require('nvim-treesitter.configs').setup {
    ensure_installed = { "c", "cpp", "lua", "javascript", "python", "vim", "json", "glsl", "html" },
    sync_install = false,
    auto_install = false,
    highlight = {
        enable = true,
    },
    indent = true,
}

-- Mason
require("mason").setup()
require("mason-lspconfig").setup({
    ensure_installed = { "clangd", "lua_ls", "ruff" },
})
vim.lsp.config("lua_ls", {
    settings = {
        Lua = {
            diagnostics = {
                globals = {
                    "vim",
                },
            },
        },
    },
})
vim.lsp.enable("ruff", false) -- python
vim.lsp.enable("lua_ls", false) -- lua
vim.lsp.enable("clangd", false) -- c and c++
vim.lsp.enable("ruff", true)
vim.lsp.enable("lua_ls", true)
vim.lsp.enable("clangd", true)

vim.diagnostic.config({
    virtual_text = true
})
--vim.lsp.config.lua_ls.setup({})

-- Saxi colortheme (my minimalistic universal colortheme)
require('saxi.init').colorscheme()

-- Vim
vim.cmd[[set nowrap]]
vim.cmd[[set expandtab]]
vim.cmd[[set tabstop=4]]
vim.cmd[[set softtabstop=4]]
vim.cmd[[set shiftwidth=4]]
vim.cmd[[set number]]
vim.cmd[[set ignorecase]]
vim.cmd('autocmd BufEnter * set formatoptions-=cro')
vim.cmd('autocmd BufEnter * setlocal formatoptions-=cro')
vim.cmd[[nmap n nzz]]
vim.cmd[[nmap p pzz]]
vim.cmd[[cle]]

vim.keymap.set('n', '<leader>q', ':qa<CR>')
vim.keymap.set('n', '<leader>fp', require('telescope.builtin').find_files)
vim.keymap.set('n', '<leader>ff', function() require('telescope.builtin').live_grep({cwd = "src"}) end)
vim.keymap.set('n', '<leader>fa', function() require('telescope.builtin').live_grep() end)
vim.keymap.set('n', '<leader>fb', require('telescope.builtin').buffers)
vim.keymap.set('n', '<leader>fg', require('telescope.builtin').resume)
vim.keymap.set('n', '<leader>tt', ':Neotree reveal_force_cwd=true action=focus position=left<CR>')
--vim.keymap.set('n', '<leader>eo', ':cnext<CR>')
--vim.keymap.set('n', '<leader>ei', ':cprev<CR>')
vim.keymap.set('n', '<C-j>', ':cnext<CR>')
vim.keymap.set('n', '<C-k>', ':cprev<CR>')
vim.keymap.set('n', 'K', vim.lsp.buf.hover)
vim.keymap.set('n', 'gd', vim.lsp.buf.definition)
vim.keymap.set('n', 'gf', require('telescope.builtin').lsp_references)
--vim.keymap.set('n', '<leader>wa', vim.lsp.buf.add_workspace_folder)
--vim.keymap.set('n', '<leader>wl', function() print(vim.inspect(vim.lsp.buf.list_workspace_folders())) end)

-- temp
vim.keymap.set('n', '<leader>in', ':tabnew ~/appdata/local/nvim/init.lua<CR>')
vim.keymap.set('n', '<leader>saxi', ':tabnew ~/appdata/local/nvim/lua/saxi/init.lua<CR>')
--vim.keymap.set('n', '<leader>b', ':tabnew | term python build.py run math0<CR>')
vim.keymap.set('n', '<leader>bv', ':tabnew | term bash scripts/runWebRelease.sh<CR>G')
vim.keymap.set('n', '<leader>bc', ':tabnew | term bash scripts/clean.sh<CR>')

function UpdateCache()
    vim.cmd("!rm -r .cache")
end

vim.keymap.set('n', '<leader>cc', UpdateCache)

function NewFile()
    local current = vim.api.nvim_buf_get_name(0)
    if current == "" then
        print("No base file — cannot determine directory.")
        return
    end

    local dir = vim.fn.fnamemodify(current, ":h")
    local new_name = vim.fn.input("New file name: ")
    if new_name == "" then
        print("Cancelled.")
        return
    end

    local new_path = vim.fs.joinpath(dir, new_name)
    vim.fn.mkdir(vim.fn.fnamemodify(new_path, ":h"), "p")

    vim.cmd("edit " .. new_path)
    vim.cmd("w")

    print("Created file: " .. new_path)
end

function DeleteCurrentFile()
    local fname = vim.api.nvim_buf_get_name(0)
    os.remove(fname)
    vim.cmd("q")
    print(string.format("File %s removed", fname))
end

function RenameCurrentFile()
    local old_path = vim.api.nvim_buf_get_name(0)
    if old_path == "" then
        print("No file associated with this buffer.")
        return
    end

    local dir = vim.fn.fnamemodify(old_path, ":h")
    local old_name = vim.fn.fnamemodify(old_path, ":t")

    local new_name = vim.fn.input("New file name: ", old_name)
    if new_name == "" or new_name == old_name then
        return
    end

    local new_path = vim.fs.joinpath(dir, new_name)

    local ok, err = os.rename(old_path, new_path)
    if not ok then
        print("Rename failed: " .. err)
        return
    end

    vim.cmd("file " .. new_path)
    vim.bo.modified = true
    vim.cmd("w!")

    print("Renamed to " .. new_name)
end

function DuplicateCurrentFile()
    local old_path = vim.api.nvim_buf_get_name(0)
    if old_path == "" then
        print("No file associated with this buffer.")
        return
    end

    local dir = vim.fn.fnamemodify(old_path, ":h")
    local old_name = vim.fn.fnamemodify(old_path, ":t")

    local new_name = vim.fn.input("Duplicate as: ", old_name)
    if new_name == "" then
        return
    end

    local new_path = vim.fs.joinpath(dir, new_name)

    local old_content = vim.fn.readfile(old_path)
    if not old_content then
        print("Failed to read original file.")
        return
    end

    local ok = vim.fn.writefile(old_content, new_path) == 0
    if not ok then
        print("Failed to write duplicated file.")
        return
    end

    vim.cmd("edit " .. new_path)

    print("Duplicated to " .. new_name)
end

function ToggleQuickfix()
    vim.cmd("cclose")
    vim.cmd("copen")
end

function CloseQuickfix()
    vim.cmd("cclose")
end

function ShowErrors()
    vim.diagnostic.setqflist()
    local qflist = vim.fn.getqflist()
    if #qflist > 0 then
        ToggleQuickfix()
    else
        CloseQuickfix()
    end
end

vim.keymap.set("n", "<leader>fn", NewFile)
vim.keymap.set("n", "<leader>fr", RenameCurrentFile)
vim.keymap.set("n", "<leader>fd", DuplicateCurrentFile)
vim.keymap.set("n", "<leader>fx", DeleteCurrentFile)
vim.keymap.set("n", "<C-q>", ToggleQuickfix)
vim.keymap.set("n", "<C-e>", CloseQuickfix)
vim.keymap.set('n', '<leader>ee', ShowErrors)

-- Alt-O
local function switch_source_header()
  local fname = vim.api.nvim_buf_get_name(0)
  if fname == "" then return end

  -- Supported header extensions
  local header_exts = { "h", "hpp", "hh", "hxx" }
  local source_exts = { "cpp", "cc", "cxx" }

  local base = fname:match("(.+)%..+$")
  local ext  = fname:match(".+%.(.+)$")

  local function try_open(exts)
    for _, e in ipairs(exts) do
      local candidate = base .. "." .. e
      if vim.fn.filereadable(candidate) == 1 then
        vim.cmd("edit " .. candidate)
        return true
      end
    end
    return false
  end

  if vim.tbl_contains(source_exts, ext) then
    -- Switch from .cpp → header
    if not try_open(header_exts) then
      print("No header file found")
    end
  elseif vim.tbl_contains(header_exts, ext) then
    -- Switch from .h → source
    if not try_open(source_exts) then
      print("No source file found")
    end
  else
    print("Not a C/C++ file")
  end
end

-- Keymap: Alt-O
vim.keymap.set("n", "<A-o>", switch_source_header, {
  desc = "Switch between .cpp and .h"
})



--vim.o.makeprg = "bash scripts/build.sh Raves Win Release"
function RunApp()
    os.remove("output/RavesWinRelease.exe")
    ToggleQuickfix()
    vim.cmd("AsyncRun bash scripts/build.sh Raves Win Release")

    vim.api.nvim_create_autocmd("User", {
        pattern = "AsyncRunStop",
        once = true,
        callback = function()
            local qflist = vim.fn.getqflist()
            local filtered = vim.tbl_filter(function(item)
                local name = vim.fn.bufname(item.bufnr)
                return item.lnum and item.lnum > 0 and item.col and item.col > 0 and name and name:match("src")
            end, qflist)
            vim.fn.setqflist(filtered, 'r')

            if #filtered > 0 then
                ToggleQuickfix()
            else
                CloseQuickfix()

                local logs = { "tunlog1", "tunlog2", "tunlog3" }
                for i = #logs, 2, -1 do
                    local old = logs[i - 1]
                    local new = logs[i]
                    if vim.fn.filereadable(old) == 1 then
                        os.rename(old, new)
                    end
                end
                -- Make sure latest log is empty
                local latest_log = logs[1]
                if vim.fn.filereadable(latest_log) == 1 then
                    os.remove(latest_log)
                end

                vim.cmd("tabnew")
                local term_cmd = string.format("bash -c './scripts/run.sh RavesWinRelease.exe | tee %s'", latest_log)
                local term_buf = vim.api.nvim_get_current_buf()
                vim.fn.termopen(term_cmd)
                --vim.cmd("term bash scripts/run.sh RavesWinRelease.exe | tee %s'")
                vim.cmd("normal! G")
                vim.api.nvim_create_autocmd("TermClose", {
                    buffer = term_buf,
                    once = true,
                    callback = function()
                        vim.cmd("tabclose")
                    end,
                })
            end
        end
    })
end

function RunLint()
    vim.cmd("args src/**/*.cpp")
    vim.cmd("argdo e")
    ShowErrors()
end

vim.keymap.set("n", "<leader>bb", RunApp)
vim.keymap.set("n", "<leader>hh", RunLint)

vim.api.nvim_create_autocmd('LspRequest', {
  callback = function(args)
    local request = args.data.request
    if request.type == 'pending' then
      print("lsp working...")
    elseif request.type == 'cancel' then
      print("lsp cancel")
    elseif request.type == 'complete' then
      print("lsp complete")
    end
  end,
})

