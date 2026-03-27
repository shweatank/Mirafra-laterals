#!/bin/bash

################################################################################
# Script: setup.sh
# Desc: Automated Git initialization, setup, and remote push
# Date: March 27, 2026
# Usage: ./setup.sh
################################################################################

set -e  # Exit on error

# ========== COLOR CODES ==========
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'  # No Color

# ========== UTILITY FUNCTIONS ==========

print_header() {
    echo ""
    echo -e "${BLUE}╔════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║         GIT SETUP AUTOMATION SCRIPT            ║${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════════════╝${NC}"
    echo ""
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_info() {
    echo -e "${BLUE}→ $1${NC}"
}

# ========== INPUT VALIDATION ==========

validate_url() {
    local url=$1
    if [[ $url =~ ^(https?|git|ssh)://.+ ]] || [[ $url =~ ^[a-zA-Z0-9._-]+@[a-zA-Z0-9.-]+:.+ ]]; then
        return 0  # Valid
    else
        return 1  # Invalid
    fi
}

validate_branch_name() {
    local branch=$1
    # Check for valid branch name (alphanumeric, hyphen, underscore, slash)
    if [[ $branch =~ ^[a-zA-Z0-9._/-]+$ ]]; then
        return 0  # Valid
    else
        return 1  # Invalid
    fi
}

# ========== MAIN LOGIC ==========

main() {
    print_header
    
    # Check if we're in a valid project directory
    if [ ! -f "Makefile" ] || [ ! -d "src" ] || [ ! -d "include" ]; then
        print_error "Not in linkedlist-project directory!"
        print_info "Please run this script from linkedlist-project root directory"
        echo "Expected to find: Makefile, src/, include/ directories"
        exit 1
    fi
    
    print_success "Found linkedlist-project structure"
    echo ""
    
    # ========== GET REPOSITORY URL ==========
    print_info "Step 1: Repository Configuration"
    echo ""
    
    while true; do
        read -p "Enter Git repository URL (https/ssh/git): " REPO_URL
        
        if [ -z "$REPO_URL" ]; then
            print_error "Repository URL cannot be empty!"
            continue
        fi
        
        if validate_url "$REPO_URL"; then
            print_success "Repository URL validated: $REPO_URL"
            break
        else
            print_error "Invalid URL format! Please use:"
            echo "  - https://github.com/user/repo.git"
            echo "  - git@github.com:user/repo.git"
            echo "  - ssh://git@github.com/user/repo.git"
        fi
    done
    echo ""
    
    # ========== GET BRANCH NAME ==========
    print_info "Step 2: Branch Configuration"
    echo ""
    
    read -p "Enter branch name (default: main): " BRANCH_NAME
    
    if [ -z "$BRANCH_NAME" ]; then
        BRANCH_NAME="main"
    fi
    
    if validate_branch_name "$BRANCH_NAME"; then
        print_success "Branch name set to: $BRANCH_NAME"
    else
        print_error "Invalid branch name!"
        echo "Branch name can only contain: letters, numbers, dots, hyphens, underscores, slashes"
        exit 1
    fi
    echo ""
    
    # ========== INITIALIZE GIT REPOSITORY ==========
    print_info "Step 3: Git Repository Initialization"
    echo ""
    
    if [ -d ".git" ]; then
        print_warning "Git repository already initialized"
        print_success "Skipping git init"
    else
        print_info "Initializing git repository..."
        git init
        print_success "Git repository initialized"
    fi
    echo ""
    
    # ========== CONFIGURE GIT (OPTIONAL) ==========
    print_info "Configuring git (optional)"
    read -p "Do you want to configure git user? (y/n, default: n): " CONFIGURE_GIT
    
    if [[ "$CONFIGURE_GIT" =~ ^[Yy]$ ]]; then
        read -p "Enter your name: " GIT_NAME
        read -p "Enter your email: " GIT_EMAIL
        
        if [ -n "$GIT_NAME" ] && [ -n "$GIT_EMAIL" ]; then
            git config user.name "$GIT_NAME"
            git config user.email "$GIT_EMAIL"
            print_success "Git configured with: $GIT_NAME <$GIT_EMAIL>"
        fi
    fi
    echo ""
    
    # ========== ADD FILES ==========
    print_info "Step 4: Adding Files"
    echo ""
    
    print_info "Running: git add ."
    git add .
    print_success "All files staged for commit"
    echo ""
    
    # ========== COMMIT CHANGES ==========
    print_info "Step 5: Creating Initial Commit"
    echo ""
    
    # Check if there are changes to commit
    if git diff-index --quiet HEAD --; then
        print_warning "No changes to commit (repository clean)"
    else
        print_info "Running: git commit -m 'Initial commit: Linked List Complete Implementation'"
        git commit -m "Initial commit: Linked List Complete Implementation

- Complete linked list implementation with all operations
- Includes insert, delete, search, traverse, reverse, sort
- Floyd's cycle detection algorithm
- Comprehensive test suite with 35+ test cases
- Interactive demo program
- Professional Makefile with build system
- Automated test logging
- Production-ready code with proper memory management"
        print_success "Initial commit created"
    fi
    echo ""
    
    # ========== CREATE/SWITCH BRANCH ==========
    print_info "Step 6: Branch Management"
    echo ""
    
    CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
    
    if [ "$CURRENT_BRANCH" != "$BRANCH_NAME" ]; then
        print_info "Current branch: $CURRENT_BRANCH"
        
        # Check if branch exists
        if git show-ref --verify --quiet refs/heads/$BRANCH_NAME; then
            print_info "Switching to existing branch: $BRANCH_NAME"
            git checkout $BRANCH_NAME
        else
            print_info "Creating and switching to new branch: $BRANCH_NAME"
            git checkout -b $BRANCH_NAME
        fi
        
        print_success "Now on branch: $BRANCH_NAME"
    else
        print_success "Already on branch: $BRANCH_NAME"
    fi
    echo ""
    
    # ========== ADD REMOTE ==========
    print_info "Step 7: Remote Repository Configuration"
    echo ""
    
    if git remote get-url origin > /dev/null 2>&1; then
        CURRENT_REMOTE=$(git remote get-url origin)
        print_warning "Remote 'origin' already exists"
        print_info "Current remote: $CURRENT_REMOTE"
        
        if [ "$CURRENT_REMOTE" != "$REPO_URL" ]; then
            read -p "Do you want to update remote URL? (y/n): " UPDATE_REMOTE
            
            if [[ "$UPDATE_REMOTE" =~ ^[Yy]$ ]]; then
                print_info "Updating remote URL..."
                git remote set-url origin "$REPO_URL"
                print_success "Remote URL updated to: $REPO_URL"
            fi
        else
            print_success "Remote URL matches provided URL"
        fi
    else
        print_info "Adding new remote 'origin'..."
        git remote add origin "$REPO_URL"
        print_success "Remote added: $REPO_URL"
    fi
    echo ""
    
    # ========== PUSH TO REMOTE ==========
    print_info "Step 8: Pushing to Remote Repository"
    echo ""
    
    read -p "Are you ready to push to remote? (y/n): " READY_TO_PUSH
    echo ""
    
    if [[ "$READY_TO_PUSH" =~ ^[Yy]$ ]]; then
        print_warning "Pushing to remote (this may require authentication)..."
        echo ""
        
        if git push -u origin $BRANCH_NAME; then
            print_success "Successfully pushed to remote!"
            print_success "Branch: $BRANCH_NAME"
            print_success "Remote: $REPO_URL"
        else
            print_error "Failed to push to remote"
            echo ""
            print_info "Troubleshooting tips:"
            echo "  1. Check internet connection"
            echo "  2. Verify repository URL: git remote -v"
            echo "  3. Check credentials and SSH/HTTPS setup"
            echo "  4. Manually push: git push -u origin $BRANCH_NAME"
            exit 1
        fi
    else
        print_warning "Push cancelled by user"
        print_info "You can manually push later with:"
        echo "  git push -u origin $BRANCH_NAME"
    fi
    echo ""
    
    # ========== SUMMARY ==========
    print_success "Setup Complete!"
    echo ""
    echo -e "${BLUE}╔════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║              SETUP SUMMARY                     ║${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════════════╝${NC}"
    echo ""
    echo "Git Configuration:"
    echo "  Repository: $REPO_URL"
    echo "  Branch:     $BRANCH_NAME"
    echo "  Local:      $(pwd)"
    echo ""
    echo "Next steps:"
    echo "  1. Make changes to code"
    echo "  2. Run: git add ."
    echo "  3. Run: git commit -m 'Your message'"
    echo "  4. Run: git push"
    echo ""
    echo "View repository status:"
    echo "  git status"
    echo "  git log"
    echo "  git remote -v"
    echo ""
    print_success "Happy coding!"
    echo ""
}

# ========== ERROR HANDLER ==========

trap 'print_error "Script interrupted"; exit 1' INT TERM

# ========== RUN MAIN ==========

if [ "$BASH_SOURCE" == "${0}" ]; then
    main "$@"
fi

################################################################################
